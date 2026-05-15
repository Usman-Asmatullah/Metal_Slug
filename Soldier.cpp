#include "Soldier.h"
#include "Enemy.h"
#include "Weapon_Types.h"
#include <cmath>

const int TransformationState::normal = 0;
const int TransformationState::undead = 1;
const int TransformationState::mummy = 2;

const float Soldier::GRAVITY = 980.0f;
const int Soldier::BASE_GRENADES = 10;
const float Soldier::BASE_PISTOL_DMG = 3.0f;
const float Soldier::BASE_KNIFE_DMG = 2.0f;
const float Soldier::BASE_FIRE_RATE = 4.0f;
const float Soldier::BASE_WALK_SPEED = 200.0f;
const float Soldier::MELEE_COOLDOWN = 0.5f;
const int Soldier::BASE_HP = 18;
const int Soldier::BASE_LIVES = 2;

const int Soldier::WEAPON_PISTOL = 0;
const int Soldier::WEAPON_HMG = 1;
const int Soldier::WEAPON_ROCKET = 2;
const int Soldier::WEAPON_FLAME = 3;
const int Soldier::WEAPON_LASER = 4;
const int Soldier::WEAPON_KNIFE = 5;

const int Soldier::WEAPON_ANIM_IDLE = 0;
const int Soldier::WEAPON_ANIM_FIRE = 1;
const int Soldier::WEAPON_ANIM_RELOAD = 2;

void UndeadState::enter(Soldier& soldier) 

{

    timer = 10.f;

}
void UndeadState::update(Soldier& soldier, float dt) 

{

    timer -= dt;

}
void UndeadState::exit(Soldier& soldier) 

{

}
void MummyState::enter(Soldier& soldier) 

{

    timer = 10.f;

}
void MummyState::update(Soldier& soldier, float dt) 

{

    timer -= dt;

}
void MummyState::exit(Soldier& soldier) 

{

}
// =============================================================================
// SOLDIER BASE CLASS
// =============================================================================
Soldier::Soldier(float x, float y, const char* name)
    : DamagableEntity(x, y, BASE_HP),
    name(name), lives(BASE_LIVES), grenades(BASE_GRENADES),
    facingRight(true), onGround(false), wasOnGround(false), useFireGrenade(false),
    velocityX(0), velocityY(0), jumpForce(-650.f),
    pistol(NULL), currentWeapon(NULL), knife(NULL),
    transformState(NULL), specialTimer(0), specialActive(false),
    animTimer(0), animFrame(0), hitWindowTimer(0), flameHoldTimer(0), flameInterrupted(false), currentWeaponAnim(0),
    renderOnlyUpper(false),
    weaponAnimTimer(0), weaponAnimFrame(0), weaponAnimState(0),
    weaponIndex(0), legAnimFrame(0), legAnimTimer(0), knifeAnimFrame(0),
    knifeAnimTimer(0), knifeAttacking(false),
    sWasDown(false), lWasDown(false), gWasDown(false), hWasDown(false)


{
    this->x = x;
    this->y = y;
    pistol = new Pistol();
    currentWeapon = pistol;
    knife = new Knife();
    transformState = new NormalState();
}

Soldier::~Soldier()


{
    if (currentWeapon != NULL && currentWeapon != pistol)
    

{
    

    delete currentWeapon;
    currentWeapon = NULL;
    

}
    if (pistol != NULL) 

{

    delete pistol;
    pistol = NULL;

}
    if (knife != NULL) 

{

    delete knife;
    knife = NULL;

}
    if (transformState != NULL) 

{

    delete transformState;
    transformState = NULL;

}
}

void Soldier::takeDamage(int amount)


{
    if (DamageState == Dead)
    {
        return;
    }
    Hp -= amount;
    if (Hp <= 0)
    

{
        Hp = 0;
        DamageState = Dead;
        alive = false;
        onDeath();
    }
    else if (Hp <= 6)
    

{
        DamageState = criticalState;
        hitWindowTimer = 1.f;
        damageTimer = damagedStateDuration;
    }
    else if (Hp <= 12)
    

{
        DamageState = injuredState;
        hitWindowTimer = 1.f;
        damageTimer = damagedStateDuration;
    }
    else
    

{
        DamageState = normalState;
        hitWindowTimer = 1.f;
        damageTimer = damagedStateDuration;
    }
}

void Soldier::setTransformationState(TransformationState* newState)


{
    if (transformState != NULL)
    

{
    

    transformState->exit(*this);
    delete transformState;
    transformState = NULL;
    

}
    transformState = newState;
    if (transformState != NULL)
    {
        transformState->enter(*this);
    }
}

int Soldier::getTransformState() const


{
    if (transformState == NULL)
    {
        return TransformationState::normal;
    }
    return transformState->getType();
}

void Soldier::pickupWeapon(Weapon* weapon)


{
    if (weapon == NULL)
    {
        return;
    }
    if (currentWeapon != NULL && currentWeapon != pistol)
    

{
        delete currentWeapon;
        currentWeapon = NULL;
    }
    currentWeapon = weapon;
    currentWeaponAnim = currentWeapon->getWeaponType();
}

void Soldier::cycleWeapon()


{
    currentWeaponAnim = WEAPON_PISTOL;
    weaponIndex = 0;
}


// Advances state calculations by frame time increment

void Soldier::update(float dt)


{
    if (hitWindowTimer > 0)
    

{
        hitWindowTimer -= dt;
        if (hitWindowTimer <= 0)
        

{
        

    hitWindowTimer = 0;
        

}
    }

    if (transformState != NULL)
    

{
        transformState->update(*this, dt);
        if (transformState->isExpired() )
        {
            setTransformationState(new NormalState());
        }
    }

    if (specialActive)
    

{
        specialTimer -= dt;
        if (specialTimer <= 0) 

{

    specialTimer = 0;
    specialActive = false;

}
    }

    bool inWater = (x >= 2080.f && x < 4000.f && y >= 500.f);
    if (!onGround) 

{
        if (inWater) 

{
            velocityY += 150.f * dt;
            if (velocityY > 100.f)
            {
                velocityY = 100.f;
            }
        }
        else 

{

    velocityY += GRAVITY * dt;

}
    }
    else if (velocityY > 0) velocityY = 0;

    float effectiveSpeed = BASE_WALK_SPEED * getSpeedMultiplier();
    if (getTransformState() == TransformationState::undead)
    {
        effectiveSpeed *= 0.5f;
    }
    if (velocityX > effectiveSpeed)
    {
        velocityX = effectiveSpeed;
    }
    if (velocityX < -effectiveSpeed)
    {
        velocityX = -effectiveSpeed;
    }
    move(velocityX * dt, velocityY * dt);

    if (currentWeapon != NULL && currentWeapon != pistol && currentWeapon->getAmmo() <= 0 && !currentWeapon->canFire())
    

{
        delete currentWeapon;
        currentWeapon = pistol;
        currentWeaponAnim = WEAPON_PISTOL;
    }
    if (currentWeapon != NULL)
    {
        currentWeapon->update(dt);
    }
    if (knife != NULL)
    {
        knife->update(dt);
    }

    if (velocityX != 0 && onGround)
    

{
        animTimer += dt;
        if (animTimer >= 0.08f)
        

{
            animFrame = (animFrame + 1) % getRunFrameCount();
            animTimer = 0;
        }
    }

    if (weaponAnimState == WEAPON_ANIM_FIRE)
    

{
        weaponAnimTimer += 0.05f;
        if (weaponAnimTimer >= 0.08f)
        

{
            weaponAnimFrame = (weaponAnimFrame + 1) % 4;
            weaponAnimTimer = 0;
            if (weaponAnimFrame == 0)
            {
                weaponAnimState = WEAPON_ANIM_IDLE;
            }
        }
    }
    else if (weaponAnimState == WEAPON_ANIM_RELOAD)
    

{
        weaponAnimTimer += 0.05f;
        if (weaponAnimTimer >= 0.3f)
        

{
            weaponAnimFrame = (weaponAnimFrame + 1) % 6;
            weaponAnimTimer = 0;
            if (weaponAnimFrame == 0)
            {
                weaponAnimState = WEAPON_ANIM_IDLE;
            }
        }
    }
    else
    

{
        weaponAnimFrame = 0;
        weaponAnimTimer = 0;
    }

    updateDamageState(dt);
}


// Draws current frame buffer pixels to visual display

void Soldier::render(sf::RenderWindow& window)


{


    render(window, 0, 0);


}

// Draws current frame buffer pixels to visual display
void Soldier::render(sf::RenderWindow& window, float camX, float camY) 

{

}
sf::FloatRect Soldier::getBounds() const 

{

    return sf::FloatRect(x, y, 90.f, 145.f);

}

// Resolves user hardware control inputs for movement
void Soldier::handleInput(float dt) 

{

}
Projectile** Soldier::shoot(float angleRad, int& outCount)


{
    if (currentWeapon == NULL || !currentWeapon->canFire()) 

{

    outCount = 0;
    return NULL;

}
    outCount = 1;
    Projectile** result = new Projectile * [1];
    result[0] = NULL;

    float spawnX;
    if (facingRight)
    
{
    
    spawnX = x + 75;
    
}
    else
    
{
    
    spawnX = x + 15;
    
}
    float spawnY = y + 68.f; // aligned perfectly with the gun level

    currentWeapon->fire(spawnX, spawnY, angleRad, result[0]);
    currentWeapon->reload();
    void registerPlayerProjectile(Projectile * p);
    registerPlayerProjectile(result[0]);

    if (weaponAnimState != WEAPON_ANIM_FIRE)
    

{
        weaponAnimState = WEAPON_ANIM_FIRE;
        weaponAnimFrame = 0;
        weaponAnimTimer = 0;
    }

    return result;
}

Grenade* Soldier::throwGrenade(float angleRad)


{
    if (grenades <= 0)
    {
        return NULL;
    }
    grenades--;
    return new Grenade();
}

bool Soldier::tryMelee(const sf::FloatRect& targetBounds)


{
    if (!canMelee() )
    {
        return false;
    }
    if (knife == NULL || !knife->canSwing() )
    {
        return false;
    }
    sf::FloatRect myBounds = getBounds();
    if (myBounds.intersects(targetBounds)) 

{

    knife->swing();
    return true;

}
    return false;
}

void Soldier::onDeath()


{
    lives--;
    if (lives > 0)
    

{
    Hp = BASE_HP;
    DamageState = normalState;
    hitWindowTimer = 0;
    velocityX = 0;
    velocityY = 0;
        setTransformationState(new NormalState());
        alive = true;
    }
}

void Soldier::reset()
{
    lives = BASE_LIVES;
    Hp = BASE_HP;
    DamageState = normalState;
    hitWindowTimer = 0;
    velocityX = 0;
    velocityY = 0;
    grenades = BASE_GRENADES;
    alive = true;
    setTransformationState(new NormalState());
    if (currentWeapon != NULL && currentWeapon != pistol)
    {
        delete currentWeapon;
        currentWeapon = pistol;
        currentWeaponAnim = WEAPON_PISTOL;
    }
}


// PLAYER SOLDIER 
PlayerSoldier::PlayerSoldier(float x, float y, const char* name)
    : Soldier(x, y, name), aimAngle(0) 

{

}

void PlayerSoldier::handleInput(float dt)


{
    bool inWater = (x >= 2080.f && x < 4000.f && y >= 500.f);
    float speed = BASE_WALK_SPEED * getSpeedMultiplier();
    if (inWater)
    {
        speed *= 0.6f;
    }
    if (getTransformState() == TransformationState::undead || getTransformState() == TransformationState::mummy)
    {
        speed *= 0.5f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    

{
    

    velocityX = speed;
    facingRight = true;
    

}
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    

{
    

    velocityX = -speed;
    facingRight = false;
    

}
    else 

{

    velocityX = 0;

}
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    

{
        if (inWater) 

{
            velocityY = -220.f; // Swim upward dynamically!
            onGround = false;
        }
        else if (onGround) 

{

    velocityY = jumpForce;
    onGround = false;

}
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) && !specialActive)
        activateSpecialPower();

    // S = shoot / melee

    bool sDown = sf::Keyboard::isKeyPressed(sf::Keyboard::S);



    if (currentWeaponAnim != WEAPON_FLAME || !sDown)
    

{
        flameHoldTimer = 0;
        flameInterrupted = false;
    }
    else if (sDown && currentWeaponAnim == WEAPON_FLAME)
    

{
        flameHoldTimer += dt;
        if (flameHoldTimer >= 5.0f)
        

{
        

    flameInterrupted = true;
        

}
    }

    if (!(getTransformState() == TransformationState::mummy && name[0] == 'E' && name[1] == 'r' && name[2] == 'i' && name[3] == '\0'))
    

{
        if ((currentWeaponAnim == WEAPON_KNIFE || getTransformState() == TransformationState::mummy) && canMelee())
        

{
            if (sDown && !sWasDown)
            

{
                weaponAnimState = WEAPON_ANIM_FIRE;
                weaponAnimFrame = 0;
                weaponAnimTimer = 0;
                knifeAttacking = true;
                knifeAnimFrame = 0;
                knifeAnimTimer = 0;
                if (GameData::activeEnemyManager)
                

{
                    int i = 0;
                    while (i < GameData::activeEnemyManager->getEnemyCount())
                    

{
                        Enemy* enemy = GameData::activeEnemyManager->getEnemy(i);
                        if (enemy && enemy->isAlive())
                        

{
                            if (getBounds().intersects(enemy->getBounds()))
                            

{
                            

    enemy->takeDamage(2); // Knife damage is 2 HP
                            

}
                        }
                        i++;
                    }
                }
            }
        }
        else
        

{
            bool canContinuousFire = (currentWeaponAnim == WEAPON_HMG || (currentWeaponAnim == WEAPON_FLAME && !flameInterrupted));
            bool tryShoot;
            if (canContinuousFire)
            
{
            
    tryShoot = sDown;
            
}
            else
            
{
            
    tryShoot = (sDown && !sWasDown);
            
}
            if (tryShoot)
            

{
                float angle;
                if (facingRight)
                
{
                
    angle = 0;
                
}
                else
                
{
                
    angle = 3.14159265f;
                
}
                int count = 0;
                Projectile** projs = shoot(angle, count);
                if (projs) 

{

    delete[] projs;

}
            }
        }
    }
    sWasDown = sDown;

    // L = Switch Grenade Type

    bool lDown = sf::Keyboard::isKeyPressed(sf::Keyboard::L);
    if (lDown && !lWasDown)
    

{
    

    useFireGrenade = !useFireGrenade;
    

}
    lWasDown = lDown;

    // G = Grenade

    bool gDown = sf::Keyboard::isKeyPressed(sf::Keyboard::G);
    if (gDown && !gWasDown && grenades > 0)
    

{
        grenades--;
        float angle;
        if (facingRight)
        
{
        
    angle = -0.5f;
        
}
        else
        
{
            angle = -2.64f;
        } // throw slightly upwards and forwards
        float speed = 500.f;
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;

        float spawnX;
        if (facingRight)
        
{
        
    spawnX = x + 75;
        
}
        else
        
{
        
    spawnX = x + 15;
        
}
        float spawnY = y + 55.f;

        sf::Color color;
        if (useFireGrenade)
        
{
        
    color = sf::Color(255, 140, 0);
        
}
        else
        
{
        
    color = sf::Color(150, 150, 150);
        
}
        Projectile* grenadeProj = new BallisticProjectile(spawnX, spawnY, vx, vy, 20, true, 192, true, color, useFireGrenade);
    void registerPlayerProjectile(Projectile * p);
    registerPlayerProjectile(grenadeProj);
    }
    gWasDown = gDown;

    // H = cycle weapon

    bool hDown = sf::Keyboard::isKeyPressed(sf::Keyboard::H);
    if (hDown && !hWasDown)
    {
        cycleWeapon();
    }
    hWasDown = hDown;
}


// All frames from "Sprites/Marco_Rossi.png"


const SpriteFrame Marco::pistolWalkFrames[5] = {
    {262,1046,37,25}, {304,1048,37,23}, {346,1048,34,26}, {385,1048,33,27}, {423,1047,32,27}
};
const SpriteFrame Marco::pistolFireFrames[6] = {
    {10,1082,48,23}, {63,1082,50,23}, {118,1082,35,23}, {173,1082,35,23}, {213,1080,36,25}, {254,1080,37,25}
};
const SpriteFrame Marco::hmgWalkFrames[5] = {
    {919,966,36,27}, {960,966,36,28}, {1001,966,36,29}, {1042,966,36,30}, {1083,966,37,30}
};
const SpriteFrame Marco::hmgFireFrames[3] = {
    {919,1088,58,27}, {982,1088,36,27}, {1047,1088,56,26}
};
const SpriteFrame Marco::knifeIdleFrames[1] = {
    {67,1249,26,27}
};
const SpriteFrame Marco::knifeAttackFrames[4] = {
    {67,1249,26,27}, {98,1250,41,23}, {144,1245,48,28}, {197,1245,48,29}
};
const SpriteFrame Marco::legFrames[4] = {
    {10,456,21,16}, {36,456,21,16}, {62,456,21,16}, {88,456,21,16}
};

Marco::Marco(float x, float y)
    : PlayerSoldier(x, y, "Marco"), dualFireActive(false), dualFireTimer(0)


{
    grenades = getStartingGrenades();
    marcoSheet.loadFromFile("Sprites/Marco_Rossi_cropped.png");
    sprite.setTexture(marcoSheet);
    legSprite.setTexture(marcoSheet);
    currentWeaponAnim = WEAPON_PISTOL;
    weaponIndex = 0;
}

int Marco::getRunFrameCount() const


{
    if (currentWeaponAnim == WEAPON_HMG)
    {
        return HMG_WALK_FRAMES;
    }
    if (currentWeaponAnim == WEAPON_KNIFE)
    {
        return KNIFE_IDLE_FRAMES;
    }
    return PISTOL_WALK_FRAMES;
}

void Marco::cycleWeapon()


{
    if (currentWeaponAnim == WEAPON_PISTOL) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = new HeavyMachineGun();
    currentWeaponAnim = WEAPON_HMG;
    weaponIndex = 1;
    }
    else if (currentWeaponAnim == WEAPON_HMG) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = new FlameShot();
    currentWeaponAnim = WEAPON_FLAME;
    weaponIndex = 3;
    }
    else if (currentWeaponAnim == WEAPON_FLAME) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = pistol;
    currentWeaponAnim = WEAPON_KNIFE;
    weaponIndex = 2;
    }
    else 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = pistol;
    currentWeaponAnim = WEAPON_PISTOL;
    weaponIndex = 0;
    }
    animFrame = 0;
    animTimer = 0;
    weaponAnimState = WEAPON_ANIM_IDLE;
    weaponAnimFrame = 0;
    weaponAnimTimer = 0;
    knifeAttacking = false;
    knifeAnimFrame = 0;
}



void Marco::render(sf::RenderWindow& window) 

{

    render(window, 0, 0);

}

void Marco::render(sf::RenderWindow& window, float camX, float camY)


{
    if (getTransformState() == TransformationState::mummy) 

{
        static sf::Texture mummyTex;
        static bool loaded = false;
        if (!loaded) 

{
            mummyTex.loadFromFile("Sprites/Marco Rossi (Mummy).png");
            loaded = true;
        }
        sf::Sprite mummySpr(mummyTex);
        sf::IntRect rect;
        if (knifeAttacking) 

{
            const SpriteFrame kFrames[2] = {
                {211, 462, 28, 37}, {132, 614, 28, 37}
            };
            rect = sf::IntRect(kFrames[knifeAnimFrame % 2].x, kFrames[knifeAnimFrame % 2].y, kFrames[knifeAnimFrame % 2].w, kFrames[knifeAnimFrame % 2].h);
        }
        else 

{

    rect = sf::IntRect(15, 690, 32, 28);

}
        mummySpr.setTextureRect(rect);

        float sc = 3.f;
        float drawX = x - camX;
        float drawY = y - camY;

        const SpriteFrame* legFrame = &legFrames[0];
        if (velocityX != 0)
            legFrame = &legFrames[legAnimFrame % LEG_FRAMES];

        float bodyW = 35.f * sc;
        float upperH = (float)rect.height * sc;
        float legH = (float)legFrame->h * sc;
        float legW = (float)legFrame->w * sc;
        float overlap = 6.f * sc;

        float totalH_draw = upperH + legH - overlap;
        float feetY_draw = drawY + 145.f;
        float upperY_draw = feetY_draw - totalH_draw;

        if (facingRight) 

{
            mummySpr.setScale(sc, sc);
            mummySpr.setPosition(drawX, upperY_draw);
        }
        else 

{
            mummySpr.setScale(-sc, sc);
            mummySpr.setPosition(drawX + bodyW, upperY_draw);
        }
        window.draw(mummySpr);

        float bodyCenterX = drawX + bodyW / 2.f;
        float legDrawX = bodyCenterX - legW / 2.f;
        float legY = upperY_draw + upperH - overlap;

        legSprite.setTexture(marcoSheet);
        legSprite.setTextureRect(sf::IntRect(legFrame->x, legFrame->y, legFrame->w, legFrame->h));
        if (!facingRight) 

{
            legSprite.setScale(-sc, sc);
            legSprite.setPosition(legDrawX + legW, legY);
        }
        else 

{
            legSprite.setScale(sc, sc);
            legSprite.setPosition(legDrawX, legY);
        }
        if (!renderOnlyUpper) window.draw(legSprite);
        return;
    }

    float sc = 3.f;
    float drawX = x - camX;
    float drawY = y - camY;

    const SpriteFrame* upperFrame = NULL;
    int upperIdx = 0;

    if (currentWeaponAnim == WEAPON_KNIFE) 

{
        if (knifeAttacking) 

{
            upperIdx = knifeAnimFrame % KNIFE_ATTACK_FRAMES;
            upperFrame = &knifeAttackFrames[upperIdx];
        }
        else 

{

    upperFrame = &knifeIdleFrames[0];

}
    }
    else if (currentWeaponAnim == WEAPON_HMG) 

{
        if (weaponAnimState == WEAPON_ANIM_FIRE) 

{
            upperIdx = weaponAnimFrame % HMG_FIRE_FRAMES;
            upperFrame = &hmgFireFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % HMG_WALK_FRAMES;
            upperFrame = &hmgWalkFrames[upperIdx];
        }
        else 

{

    upperFrame = &hmgWalkFrames[0];

}
    }
    else 

{ // PISTOL
        if (weaponAnimState == WEAPON_ANIM_FIRE) 

{
            upperIdx = weaponAnimFrame % PISTOL_FIRE_FRAMES;
            upperFrame = &pistolFireFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % PISTOL_WALK_FRAMES;
            upperFrame = &pistolWalkFrames[upperIdx];
        }
        else 

{

    upperFrame = &pistolWalkFrames[0];

}
    }

    const SpriteFrame* legFrame = &legFrames[0];
    if (velocityX != 0)
        legFrame = &legFrames[legAnimFrame % LEG_FRAMES];

    float bodyW;
    if (currentWeaponAnim == WEAPON_KNIFE)
    {
        bodyW = 26.f * sc;
    }
    else if (currentWeaponAnim == WEAPON_HMG) bodyW = 36.f * sc;
    else bodyW = 35.f * sc;

    float upperH = (float)upperFrame->h * sc;
    float upperW = (float)upperFrame->w * sc;
    float legH = (float)legFrame->h * sc;
    float legW = (float)legFrame->w * sc;

    // Overlap
    float overlapPx;
    if (currentWeaponAnim == WEAPON_KNIFE)
    {
        overlapPx = -3.f;
    }
    else if (currentWeaponAnim == WEAPON_HMG) overlapPx = 0;
    else overlapPx = 1.f;
    float overlap = overlapPx * sc;

    // Knife: crop bottom 10px
    int cropBottom;
    if (currentWeaponAnim == WEAPON_KNIFE)
    
{
    
    cropBottom = 10;
    
}
    else
    
{
    
    cropBottom = 0;
    
}
    int croppedH = upperFrame->h - cropBottom;
    float croppedUpperH = (float)croppedH * sc;

    float totalH_draw = croppedUpperH + legH - overlap;
    float feetY_draw = drawY + 145.f;
    float upperY_draw = feetY_draw - totalH_draw;

    sprite.setTexture(marcoSheet);
    sprite.setTextureRect(sf::IntRect(upperFrame->x, upperFrame->y, upperFrame->w, croppedH));
    if (!facingRight) 

{
        sprite.setScale(-sc, sc);
        sprite.setPosition(drawX + bodyW, upperY_draw);
    }
    else 

{
        sprite.setScale(sc, sc);
        sprite.setPosition(drawX, upperY_draw);
    }
    window.draw(sprite);

    if (currentWeaponAnim == WEAPON_FLAME && weaponAnimState == WEAPON_ANIM_FIRE && !flameInterrupted) 

{
        static sf::Texture flameTex;
        static bool loaded = false;
        if (!loaded) 

{
            flameTex.loadFromFile("Sprites/Player/flame.png");
            loaded = true;
        }
        sf::Sprite flameSpr(flameTex);
        flameSpr.setScale(sc * 0.35f, sc * 0.5f);

        static float renderTimer = 0;
        static int renderFrame = 0;
        renderTimer += 0.016f;
        if (renderTimer >= 0.08f) 

{
            renderTimer = 0;
            renderFrame = (renderFrame + 1) % 8;
        }
        int frameW = flameTex.getSize().x / 8;
        if (frameW <= 0)
        {
            frameW = 32;
        }
        int frameH = flameTex.getSize().y;
        flameSpr.setTextureRect(sf::IntRect(renderFrame * frameW, 0, frameW, frameH));
        flameSpr.setOrigin(0, (float)frameH / 2.f);

        if (facingRight) 

{

    flameSpr.setPosition(drawX + bodyW - 2.f, upperY_draw + 34.f);

}
        else 

{
            flameSpr.setScale(-sc * 0.35f, sc * 0.5f);
            flameSpr.setPosition(drawX + 2.f, upperY_draw + 34.f);
        }
        window.draw(flameSpr);
    }

    upperH = croppedUpperH;
    float bodyCenterX = drawX + bodyW / 2.f;
    float legDrawX = bodyCenterX - legW / 2.f;
    float legY = upperY_draw + upperH - overlap;

    legSprite.setTexture(marcoSheet);
    legSprite.setTextureRect(sf::IntRect(legFrame->x, legFrame->y, legFrame->w, legFrame->h));
    if (!facingRight) 

{
        legSprite.setScale(-sc, sc);
        legSprite.setPosition(legDrawX + legW, legY);
    }
    else 

{
        legSprite.setScale(sc, sc);
        legSprite.setPosition(legDrawX, legY);
    }
    if (!renderOnlyUpper) window.draw(legSprite);
}

void Marco::activateSpecialPower()


{


    dualFireActive = true;
    dualFireTimer = 10.f;
    specialActive = true;
    specialTimer = 10.f;


}

void Marco::update(float dt)


{
    if (getTransformState() == TransformationState::mummy) 

{
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && currentWeaponAnim == WEAPON_KNIFE) 

{
            if (!knifeAttacking) 

{

    knifeAttacking = true;
    knifeAnimFrame = 0;
    knifeAnimTimer = 0;

}
        }
        if (knifeAttacking) 

{
            knifeAnimTimer += dt;
            if (knifeAnimTimer >= 0.08f) 

{
                knifeAnimTimer = 0;
                knifeAnimFrame++;
                if (knifeAnimFrame >= 5) 

{

    knifeAttacking = false;
    knifeAnimFrame = 0;

}
            }
        }
        else if (velocityX != 0) 

{
            animTimer += dt;
            if (animTimer >= 0.15f) 

{
                animTimer = 0;
                animFrame = (animFrame + 1) % 4;
            }
        }
        else 

{

    animFrame = 0;

}
        Soldier::update(dt);
        return;
    }

    if (dualFireActive) 

{
        dualFireTimer -= dt;
        if (dualFireTimer <= 0) 

{

    dualFireActive = false;
    dualFireTimer = 0;

}
    }
    if (velocityX != 0 && onGround) 

{
        legAnimTimer += dt;
        if (legAnimTimer >= 0.08f) 

{
            legAnimFrame = (legAnimFrame + 1) % LEG_FRAMES;
            legAnimTimer = 0;
        }
    }
    else 

{

    legAnimFrame = 0;
    legAnimTimer = 0;

}
    if (knifeAttacking) 

{
        knifeAnimTimer += dt;
        if (knifeAnimTimer >= 0.07f) 

{
            knifeAnimFrame++;
            knifeAnimTimer = 0;
            if (knifeAnimFrame >= KNIFE_ATTACK_FRAMES) 

{

    knifeAttacking = false;
    knifeAnimFrame = 0;

}
        }
    }
    if (currentWeaponAnim == WEAPON_KNIFE && weaponAnimState == WEAPON_ANIM_FIRE) 

{
        if (!knifeAttacking) 

{

    knifeAttacking = true;
    knifeAnimFrame = 0;
    knifeAnimTimer = 0;

}
        weaponAnimState = WEAPON_ANIM_IDLE;
    }
    Soldier::update(dt);
}

Projectile** Marco::shoot(float angleRad, int& outCount)


{
    if (currentWeapon == NULL || !currentWeapon->canFire()) 

{

    outCount = 0;
    return NULL;

}
    if (dualFireActive) 

{
        outCount = 2;
        Projectile** result = new Projectile * [2];
    result[0] = NULL;
    result[1] = NULL;

        float spawnX;
        if (facingRight)
        
{
        
    spawnX = x + 75;
        
}
        else
        
{
        
    spawnX = x + 15;
        
}
        float spawnY = y + 68.f; // aligned perfectly with the gun level

        currentWeapon->fire(spawnX, spawnY, angleRad, result[0]);
        currentWeapon->fire(spawnX, spawnY, angleRad + 3.14159265f, result[1]);
        currentWeapon->reload();
    void registerPlayerProjectile(Projectile * p);
    registerPlayerProjectile(result[0]);
        registerPlayerProjectile(result[1]);

    weaponAnimState = WEAPON_ANIM_FIRE;
    weaponAnimFrame = 0;
    weaponAnimTimer = 0;
        return result;
    }
    return Soldier::shoot(angleRad, outCount);
}

// =============================================================================
// =============================================================================

const SpriteFrame Eri::pistolWalkFrames[12] = {
    {26,537,32,28}, {61,538,32,28}, {96,539,31,28}, {130,539,31,28},
    {164,539,31,28}, {200,539,31,28}, {236,540,31,28}, {271,540,31,28},
    {306,540,32,28}, {343,540,32,28}, {379,540,30,29}, {413,540,30,29}
};

const SpriteFrame Eri::legFrames[15] = {
    {36,574,25,24}, {68,574,25,24}, {99,574,23,24}, {127,573,24,25},
    {155,572,18,26}, {178,572,20,26}, {203,572,22,26}, {232,572,25,25},
    {265,573,27,23}, {297,572,25,26}, {326,572,17,26}, {349,573,18,25},
    {373,573,21,25}, {400,574,26,23}, {431,574,30,25}
};

const SpriteFrame Eri::pistolFireFrames[9] = {
    {4,895,51,21}, {61,894,35,22}, {119,895,34,21}, {178,895,32,21},
    {216,894,31,22}, {255,896,36,20}, {296,896,36,20}, {337,895,36,22},
    {378,895,36,25}
};

const SpriteFrame Eri::hmgWalkFrames[7] = {
    {81,2474,38,28}, {153,2475,63,26}, {229,2476,38,26}, {300,2476,43,26},
    {351,2475,40,27}, {400,2475,38,27}, {81,2474,38,28}
};

const SpriteFrame Eri::hmgFireFrames[12] = {
    {6,2162,43,26}, {55,2162,43,26}, {104,2162,42,26}, {154,2162,42,26},
    {201,2162,42,26}, {248,2162,42,26}, {296,2162,42,26}, {344,2162,42,26},
    {391,2162,43,26}, {440,2162,43,26}, {489,2162,42,26}, {539,2162,42,26}
};

const SpriteFrame Eri::knifeFrames[9] = {
    {8,3337,30,28}, {48,3337,30,28}, {88,3337,32,28}, {132,3336,48,29},
    {192,3333,45,32}, {246,3333,46,32}, {302,3335,40,30}, {352,3337,28,28},
    {393,3337,28,28}
};

Eri::Eri(float x, float y)
    : PlayerSoldier(x, y, "Eri"), doubleGrenadeActive(false), doubleGrenadeTimer(0)


{
    grenades = getStartingGrenades();
    eriSheet.loadFromFile("Sprites/Eri Kasamoto.png");
    sprite.setTexture(eriSheet);
    legSprite.setTexture(eriSheet);
    currentWeaponAnim = WEAPON_PISTOL;
    weaponIndex = 0;
}

int Eri::getRunFrameCount() const


{
    if (currentWeaponAnim == WEAPON_HMG)
    {
        return HMG_WALK_FRAMES;
    }
    return PISTOL_WALK_FRAMES;
}

void Eri::cycleWeapon()


{
    if (getTransformState() == TransformationState::mummy)
    {
        return;
    }

    if (currentWeaponAnim == WEAPON_PISTOL) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = new HeavyMachineGun();
        currentWeaponAnim = WEAPON_HMG;
        weaponIndex = 1;
    }
    else if (currentWeaponAnim == WEAPON_HMG) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = new FlameShot();
        currentWeaponAnim = WEAPON_FLAME;
        weaponIndex = 3;
    }
    else 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = pistol;
        currentWeaponAnim = WEAPON_PISTOL;
        weaponIndex = 0;
    }
    animFrame = 0;
    animTimer = 0;
    weaponAnimState = WEAPON_ANIM_IDLE;
    weaponAnimFrame = 0;
    weaponAnimTimer = 0;
    knifeAttacking = false;
    knifeAnimFrame = 0;
}


// Draws current frame buffer pixels to visual display

void Eri::render(sf::RenderWindow& window)


{


    render(window, 0, 0);


}

// Draws current frame buffer pixels to visual display
void Eri::render(sf::RenderWindow& window, float camX, float camY)


{
    if (getTransformState() == TransformationState::mummy) 

{
        static sf::Texture mummyTex;
        static bool loaded = false;
        if (!loaded) 

{
            mummyTex.loadFromFile("Sprites/Marco Rossi (Mummy).png");
            loaded = true;
        }
        sf::Sprite mummySpr(mummyTex);
        sf::IntRect rect;
        if (knifeAttacking) 

{
            const SpriteFrame kFrames[2] = {
                {211, 462, 28, 37}, {132, 614, 28, 37}
            };
            rect = sf::IntRect(kFrames[knifeAnimFrame % 2].x, kFrames[knifeAnimFrame % 2].y, kFrames[knifeAnimFrame % 2].w, kFrames[knifeAnimFrame % 2].h);
        }
        else 

{

    rect = sf::IntRect(15, 690, 32, 28);

}
        mummySpr.setTextureRect(rect);

        float sc = 3.f;
        float drawX = x - camX;
        float drawY = y - camY;

        const SpriteFrame* legFrame = NULL;
        int legIdx = 0;
        if (velocityX != 0) 

{

    legIdx = legAnimFrame % LEG_FRAMES;

}
        else 

{

    legIdx = 0;

}
        legFrame = &legFrames[legIdx];

        float bodyW = 31.f * sc;
        float upperH = (float)rect.height * sc;
        float legH = (float)legFrame->h * sc;
        float legW = (float)legFrame->w * sc;
        float overlap = 6.f * sc;

        float totalH_draw = upperH + legH - overlap;
        float feetY_draw = drawY + 145.f;
        float upperY_draw = feetY_draw - totalH_draw;

        if (facingRight) 

{
            mummySpr.setScale(sc, sc);
            mummySpr.setPosition(drawX, upperY_draw);
        }
        else 

{
            mummySpr.setScale(-sc, sc);
            mummySpr.setPosition(drawX + bodyW, upperY_draw);
        }
        window.draw(mummySpr);

        float bodyCenterX = drawX + bodyW / 2.f;
        float legDrawX = bodyCenterX - legW / 2.f;
        float legY = upperY_draw + upperH - overlap;

        legSprite.setTexture(eriSheet);
        legSprite.setTextureRect(sf::IntRect(legFrame->x, legFrame->y, legFrame->w, legFrame->h));
        if (!facingRight) 

{
            legSprite.setScale(-sc, sc);
            legSprite.setPosition(legDrawX + legW, legY);
        }
        else 

{
            legSprite.setScale(sc, sc);
            legSprite.setPosition(legDrawX, legY);
        }
        if (!renderOnlyUpper) window.draw(legSprite);
        return;
    }

    float sc = 3.f;
    float drawX = x - camX;
    float drawY = y - camY;

    const SpriteFrame* upperFrame = NULL;
    int upperIdx = 0;

    if (currentWeaponAnim == WEAPON_KNIFE)
    

{
        if (knifeAttacking) 

{

    upperIdx = knifeAnimFrame % KNIFE_FRAMES;

}
        else 

{

    upperIdx = 0;

}
        upperFrame = &knifeFrames[upperIdx];
    }
    else if (currentWeaponAnim == WEAPON_HMG)
    

{
        if (weaponAnimState == WEAPON_ANIM_FIRE) 

{
            upperIdx = weaponAnimFrame % HMG_WALK_FRAMES;
            upperFrame = &hmgWalkFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % HMG_FIRE_FRAMES;
            upperFrame = &hmgFireFrames[upperIdx];
        }
        else 

{
            upperIdx = 0;
            upperFrame = &hmgFireFrames[0];
        }
    }
    else // PISTOL
    {
        if (weaponAnimState == WEAPON_ANIM_FIRE) 

{
            upperIdx = weaponAnimFrame % PISTOL_FIRE_FRAMES;
            upperFrame = &pistolFireFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % PISTOL_WALK_FRAMES;
            upperFrame = &pistolWalkFrames[upperIdx];
        }
        else 

{
            upperIdx = 0;
            upperFrame = &pistolWalkFrames[0];
        }
    }

    const SpriteFrame* legFrame = NULL;
    int legIdx = 0;
    if (velocityX != 0) 

{

    legIdx = legAnimFrame % LEG_FRAMES;

}
    else 

{

    legIdx = 0;

}
    legFrame = &legFrames[legIdx];

    float bodyW;
    if (currentWeaponAnim == WEAPON_KNIFE)
        bodyW = 30.f * sc;   // knife idle width
    else if (currentWeaponAnim == WEAPON_HMG)
        bodyW = 42.f * sc;   // hmg body width
    else
        bodyW = 31.f * sc;   // pistol body width

    float upperH = (float)upperFrame->h * sc;
    float upperW = (float)upperFrame->w * sc;
    float legH = (float)legFrame->h * sc;
    float legW = (float)legFrame->w * sc;

    float overlapPx;
    if (currentWeaponAnim == WEAPON_KNIFE)
        overlapPx = 2.f;
    else if (currentWeaponAnim == WEAPON_HMG)
        overlapPx = 3.f;
    else
        overlapPx = 6.f;   // pistol: more overlap to bring body lower
    float overlap = overlapPx * sc;
    float totalH = upperH + legH - overlap;
    float feetY = drawY + 145.f;
    float upperY = feetY - totalH;

    sprite.setTexture(eriSheet);
    int cropBottom;
    if (currentWeaponAnim == WEAPON_KNIFE)
    
{
    
    cropBottom = 10;
    
}
    else
    
{
    
    cropBottom = 0;
    
}
    int croppedH = upperFrame->h - cropBottom;
    float croppedUpperH = (float)croppedH * sc;
    sprite.setTextureRect(sf::IntRect(upperFrame->x, upperFrame->y, upperFrame->w, croppedH));

    float totalH_draw = croppedUpperH + legH - overlap;
    float feetY_draw = drawY + 145.f;
    float upperY_draw = feetY_draw - totalH_draw;

    if (!facingRight) 

{
        sprite.setScale(-sc, sc);
        sprite.setPosition(drawX + bodyW, upperY_draw);
    }
    else 

{
        sprite.setScale(sc, sc);
        sprite.setPosition(drawX, upperY_draw);
    }
    window.draw(sprite);

    if (currentWeaponAnim == WEAPON_FLAME && weaponAnimState == WEAPON_ANIM_FIRE && !flameInterrupted) 

{
        static sf::Texture flameTex;
        static bool loaded = false;
        if (!loaded) 

{
            flameTex.loadFromFile("Sprites/Player/flame.png");
            loaded = true;
        }
        sf::Sprite flameSpr(flameTex);
        flameSpr.setScale(sc * 0.35f, sc * 0.5f);

        static float renderTimer = 0;
        static int renderFrame = 0;
        renderTimer += 0.016f;
        if (renderTimer >= 0.08f) 

{
            renderTimer = 0;
            renderFrame = (renderFrame + 1) % 8;
        }
        int frameW = flameTex.getSize().x / 8;
        if (frameW <= 0)
        {
            frameW = 32;
        }
        int frameH = flameTex.getSize().y;
        flameSpr.setTextureRect(sf::IntRect(renderFrame * frameW, 0, frameW, frameH));
        flameSpr.setOrigin(0, (float)frameH / 2.f);

        if (facingRight) 

{

    flameSpr.setPosition(drawX + bodyW - 2.f, upperY_draw + 34.f);

}
        else 

{
            flameSpr.setScale(-sc * 0.35f, sc * 0.5f);
            flameSpr.setPosition(drawX + 2.f, upperY_draw + 34.f);
        }
        window.draw(flameSpr);
    }

    upperH = croppedUpperH;

    float bodyCenterX = drawX + bodyW / 2.f;
    float legDrawX = bodyCenterX - legW / 2.f;
    float legY = upperY_draw + upperH - overlap;

    legSprite.setTexture(eriSheet);
    legSprite.setTextureRect(sf::IntRect(legFrame->x, legFrame->y, legFrame->w, legFrame->h));
    if (!facingRight) 

{
        legSprite.setScale(-sc, sc);
        legSprite.setPosition(legDrawX + legW, legY);
    }
    else 

{
        legSprite.setScale(sc, sc);
        legSprite.setPosition(legDrawX, legY);
    }
    if (!renderOnlyUpper) window.draw(legSprite);
}


// Advances state calculations by frame time increment

void Eri::update(float dt)


{
    if (getTransformState() == TransformationState::mummy) 

{
        knifeAttacking = false;
        if (velocityX != 0) 

{
            animTimer += dt;
            if (animTimer >= 0.15f) 

{
                animTimer = 0;
                animFrame = (animFrame + 1) % 3;
            }
        }
        else 

{

    animFrame = 0;

}
        Soldier::update(dt);
        return;
    }

    if (doubleGrenadeActive) 

{
        doubleGrenadeTimer -= dt;
        if (doubleGrenadeTimer <= 0) 

{

    doubleGrenadeActive = false;
    doubleGrenadeTimer = 0;

}
    }

    if (velocityX != 0 && onGround) 

{
        legAnimTimer += dt;
        if (legAnimTimer >= 0.06f) 

{
            legAnimFrame = (legAnimFrame + 1) % LEG_FRAMES;
            legAnimTimer = 0;
        }
    }
    else 

{
        legAnimFrame = 0;
        legAnimTimer = 0;
    }

    // Knife attack animation
    if (knifeAttacking) 

{
        knifeAnimTimer += dt;
        if (knifeAnimTimer >= 0.07f) 

{
            knifeAnimFrame++;
            knifeAnimTimer = 0;
            if (knifeAnimFrame >= KNIFE_FRAMES) 

{
                knifeAttacking = false;
                knifeAnimFrame = 0;
            }
        }
    }

    if (currentWeaponAnim == WEAPON_KNIFE && weaponAnimState == WEAPON_ANIM_FIRE) 

{
        if (!knifeAttacking) 

{
            knifeAttacking = true;
            knifeAnimFrame = 0;
            knifeAnimTimer = 0;
        }
        weaponAnimState = WEAPON_ANIM_IDLE;
    }

    Soldier::update(dt);
}

void Eri::activateSpecialPower()


{


    doubleGrenadeActive = true;
    doubleGrenadeTimer = 10.f;
    specialActive = true;
    specialTimer = 10.f;


}
Grenade* Eri::throwGrenade(float angle)


{
    if (grenades <= 0)
    {
        return NULL;
    }
    grenades--;
    return new FireBombGrenade();
}


// All frames from "Sprites/Fiolina_Germi_cropped.png"


const SpriteFrame Fio::pistolWalkFrames[4] = {
    {242,1032,33,19}, {280,1032,33,19}, {356,1032,32,23}, {393,1031,30,25}
};
const SpriteFrame Fio::legFrames[4] = {
    {97,634,23,24}, {10,634,22,24}, {152,603,18,26}, {117,604,30,25}
};
const SpriteFrame Fio::pistolFireFrames[6] = {
    {10,1032,48,19}, {63,1032,33,19}, {118,1031,31,20}, {174,1030,29,21},
    {208,1029,29,22}, {242,1032,33,19}
};
const SpriteFrame Fio::hmgWalkFrames[5] = {
    {10,3355,38,24}, {53,3355,38,24}, {96,3356,38,24}, {139,3356,38,24}, {182,3355,38,24}
};
const SpriteFrame Fio::hmgFireFrames[4] = {
    {10,3615,60,25}, {75,3616,35,25}, {139,3616,59,24}, {203,3615,34,26}
};
const SpriteFrame Fio::knifeWalkFrames[3] = {
    {346,5387,28,27}, {219,5382,45,32}, {170,5382,44,32}
};
const SpriteFrame Fio::knifeAttackFrames[4] = {
    {80,5386,32,28}, {117,5385,48,29}, {269,5384,39,30}, {313,5386,28,28}
};

Fio::Fio(float x, float y)
    : PlayerSoldier(x, y, "Fio"), extraAmmoActive(false), extraAmmoTimer(0)


{
    grenades = getStartingGrenades();
    fioSheet.loadFromFile("Sprites/Fiolina_Germi_cropped.png");
    sprite.setTexture(fioSheet);
    legSprite.setTexture(fioSheet);
    currentWeaponAnim = WEAPON_PISTOL;
    weaponIndex = 0;
}

int Fio::getRunFrameCount() const


{
    if (currentWeaponAnim == WEAPON_HMG)
    {
        return HMG_WALK_FRAMES;
    }
    if (currentWeaponAnim == WEAPON_KNIFE)
    {
        return KNIFE_WALK_FRAMES;
    }
    return PISTOL_WALK_FRAMES;
}

void Fio::cycleWeapon()


{
    if (currentWeaponAnim == WEAPON_PISTOL) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = new HeavyMachineGun();
    currentWeaponAnim = WEAPON_HMG;
    weaponIndex = 1;
    }
    else if (currentWeaponAnim == WEAPON_HMG) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = new FlameShot();
    currentWeaponAnim = WEAPON_FLAME;
    weaponIndex = 3;
    }
    else if (currentWeaponAnim == WEAPON_FLAME) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = pistol;
    currentWeaponAnim = WEAPON_KNIFE;
    weaponIndex = 2;
    }
    else 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = pistol;
    currentWeaponAnim = WEAPON_PISTOL;
    weaponIndex = 0;
    }
    animFrame = 0;
    animTimer = 0;
    weaponAnimState = WEAPON_ANIM_IDLE;
    weaponAnimFrame = 0;
    weaponAnimTimer = 0;
    knifeAttacking = false;
    knifeAnimFrame = 0;
}



void Fio::render(sf::RenderWindow& window) 

{

    render(window, 0, 0);

}


void Fio::render(sf::RenderWindow& window, float camX, float camY)


{
    if (getTransformState() == TransformationState::mummy) 

{
        static sf::Texture marcoTex;
        static bool loaded = false;
        if (!loaded) 

{
            marcoTex.loadFromFile("Sprites/Marco Rossi (Mummy).png");
            loaded = true;
        }
        sf::Sprite mummySpr(marcoTex);
        sf::IntRect rect;
        if (knifeAttacking) 

{
            const SpriteFrame kFrames[2] = {
                {211, 462, 28, 37}, {132, 614, 28, 37}
            };
            rect = sf::IntRect(kFrames[knifeAnimFrame % 2].x, kFrames[knifeAnimFrame % 2].y, kFrames[knifeAnimFrame % 2].w, kFrames[knifeAnimFrame % 2].h);
        }
        else 

{

    rect = sf::IntRect(15, 690, 32, 28);

}
        mummySpr.setTextureRect(rect);

        float sc = 3.f;
        float drawX = x - camX;
        float drawY = y - camY;

        const SpriteFrame* legFrame = &legFrames[0];
        if (velocityX != 0)
            legFrame = &legFrames[legAnimFrame % LEG_FRAMES];

        float bodyW = 31.f * sc;
        float upperH = (float)rect.height * sc;
        float legH = (float)legFrame->h * sc;
        float legW = (float)legFrame->w * sc;
        float overlap = 6.f * sc;

        float totalH_draw = upperH + legH - overlap;
        float feetY_draw = drawY + 145.f;
        float upperY_draw = feetY_draw - totalH_draw;

        if (facingRight) 

{
            mummySpr.setScale(sc, sc);
            mummySpr.setPosition(drawX, upperY_draw);
        }
        else 

{
            mummySpr.setScale(-sc, sc);
            mummySpr.setPosition(drawX + bodyW, upperY_draw);
        }
        window.draw(mummySpr);

        float bodyCenterX = drawX + bodyW / 2.f;
        float legDrawX = bodyCenterX - legW / 2.f;
        float legY = upperY_draw + upperH - overlap;

        legSprite.setTexture(fioSheet);
        legSprite.setTextureRect(sf::IntRect(legFrame->x, legFrame->y, legFrame->w, legFrame->h));
        if (!facingRight) 

{
            legSprite.setScale(-sc, sc);
            legSprite.setPosition(legDrawX + legW, legY);
        }
        else 

{
            legSprite.setScale(sc, sc);
            legSprite.setPosition(legDrawX, legY);
        }
        if (!renderOnlyUpper) window.draw(legSprite);
        return;
    }

    float sc = 3.f;
    float drawX = x - camX;
    float drawY = y - camY;

    const SpriteFrame* upperFrame = NULL;
    int upperIdx = 0;

    if (currentWeaponAnim == WEAPON_KNIFE) 

{
        if (knifeAttacking) 

{
            upperIdx = knifeAnimFrame % KNIFE_ATTACK_FRAMES;
            upperFrame = &knifeAttackFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % KNIFE_WALK_FRAMES;
            upperFrame = &knifeWalkFrames[upperIdx];
        }
        else 

{

    upperFrame = &knifeWalkFrames[0];

}
    }
    else if (currentWeaponAnim == WEAPON_HMG) 

{
        if (weaponAnimState == WEAPON_ANIM_FIRE) 

{
            upperIdx = weaponAnimFrame % HMG_FIRE_FRAMES;
            upperFrame = &hmgFireFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % HMG_WALK_FRAMES;
            upperFrame = &hmgWalkFrames[upperIdx];
        }
        else 

{

    upperFrame = &hmgWalkFrames[0];

}
    }
    else 

{
        if (weaponAnimState == WEAPON_ANIM_FIRE) 

{
            upperIdx = weaponAnimFrame % PISTOL_FIRE_FRAMES;
            upperFrame = &pistolFireFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % PISTOL_WALK_FRAMES;
            upperFrame = &pistolWalkFrames[upperIdx];
        }
        else 

{

    upperFrame = &pistolWalkFrames[0];

}
    }

    const SpriteFrame* legFrame = &legFrames[0];
    if (velocityX != 0)
        legFrame = &legFrames[legAnimFrame % LEG_FRAMES];

    float bodyW;
    if (currentWeaponAnim == WEAPON_KNIFE)
    {
        bodyW = 28.f * sc;
    }
    else if (currentWeaponAnim == WEAPON_HMG) bodyW = 38.f * sc;
    else bodyW = 33.f * sc;

    float upperH = (float)upperFrame->h * sc;
    float legH = (float)legFrame->h * sc;
    float legW = (float)legFrame->w * sc;

    float overlapPx;
    if (currentWeaponAnim == WEAPON_KNIFE)
    {
        overlapPx = 2.f;
    }
    else if (currentWeaponAnim == WEAPON_HMG) overlapPx = 3.f;
    else overlapPx = 6.f;
    float overlap = overlapPx * sc;

    int cropBottom;
    if (currentWeaponAnim == WEAPON_KNIFE)
    
{
    
    cropBottom = 10;
    
}
    else
    
{
    
    cropBottom = 0;
    
}
    int croppedH = upperFrame->h - cropBottom;
    float croppedUpperH = (float)croppedH * sc;

    float totalH_draw = croppedUpperH + legH - overlap;
    float feetY_draw = drawY + 145.f;
    float upperY_draw = feetY_draw - totalH_draw;

    sprite.setTexture(fioSheet);
    sprite.setTextureRect(sf::IntRect(upperFrame->x, upperFrame->y, upperFrame->w, croppedH));
    if (!facingRight) 

{
        sprite.setScale(-sc, sc);
        sprite.setPosition(drawX + bodyW, upperY_draw);
    }
    else 

{
        sprite.setScale(sc, sc);
        sprite.setPosition(drawX, upperY_draw);
    }
    window.draw(sprite);

    if (currentWeaponAnim == WEAPON_FLAME && weaponAnimState == WEAPON_ANIM_FIRE && !flameInterrupted) 

{
        static sf::Texture flameTex;
        static bool loaded = false;
        if (!loaded) 

{
            flameTex.loadFromFile("Sprites/Player/flame.png");
            loaded = true;
        }
        sf::Sprite flameSpr(flameTex);
        flameSpr.setScale(sc * 0.35f, sc * 0.5f);

        static float renderTimer = 0;
        static int renderFrame = 0;
        renderTimer += 0.016f;
        if (renderTimer >= 0.08f) 

{
            renderTimer = 0;
            renderFrame = (renderFrame + 1) % 8;
        }
        int frameW = flameTex.getSize().x / 8;
        if (frameW <= 0)
        {
            frameW = 32;
        }
        int frameH = flameTex.getSize().y;
        flameSpr.setTextureRect(sf::IntRect(renderFrame * frameW, 0, frameW, frameH));
        flameSpr.setOrigin(0, (float)frameH / 2.f);

        if (facingRight) 

{

    flameSpr.setPosition(drawX + bodyW - 2.f, upperY_draw + 34.f);

}
        else 

{
            flameSpr.setScale(-sc * 0.35f, sc * 0.5f);
            flameSpr.setPosition(drawX + 2.f, upperY_draw + 34.f);
        }
        window.draw(flameSpr);
    }

    upperH = croppedUpperH;
    float bodyCenterX = drawX + bodyW / 2.f;
    float legDrawX = bodyCenterX - legW / 2.f;
    float legY = upperY_draw + upperH - overlap;

    legSprite.setTexture(fioSheet);
    legSprite.setTextureRect(sf::IntRect(legFrame->x, legFrame->y, legFrame->w, legFrame->h));
    if (!facingRight) 

{
        legSprite.setScale(-sc, sc);
        legSprite.setPosition(legDrawX + legW, legY);
    }
    else 

{
        legSprite.setScale(sc, sc);
        legSprite.setPosition(legDrawX, legY);
    }
    if (!renderOnlyUpper) window.draw(legSprite);
}

void Fio::activateSpecialPower()


{


    extraAmmoActive = true;
    extraAmmoTimer = 10.f;
    specialActive = true;
    specialTimer = 10.f;


}


void Fio::update(float dt)


{
    if (getTransformState() == TransformationState::mummy) 

{
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && currentWeaponAnim == WEAPON_KNIFE) 

{
            if (!knifeAttacking) 

{

    knifeAttacking = true;
    knifeAnimFrame = 0;
    knifeAnimTimer = 0;

}
        }
        if (knifeAttacking) 

{
            knifeAnimTimer += dt;
            if (knifeAnimTimer >= 0.08f) 

{
                knifeAnimTimer = 0;
                knifeAnimFrame++;
                if (knifeAnimFrame >= 5) 

{

    knifeAttacking = false;
    knifeAnimFrame = 0;

}
            }
        }
        else if (velocityX != 0) 

{
            animTimer += dt;
            if (animTimer >= 0.15f) 

{
                animTimer = 0;
                animFrame = (animFrame + 1) % 3;
            }
        }
        else 

{

    animFrame = 0;

}
        Soldier::update(dt);
        return;
    }

    if (extraAmmoActive) 

{
        extraAmmoTimer -= dt;
        if (extraAmmoTimer <= 0) 

{

    extraAmmoActive = false;
    extraAmmoTimer = 0;

}
    }
    if (velocityX != 0 && onGround) 

{
        legAnimTimer += dt;
        if (legAnimTimer >= 0.08f) 

{
            legAnimFrame = (legAnimFrame + 1) % LEG_FRAMES;
            legAnimTimer = 0;
        }
    }
    else 

{

    legAnimFrame = 0;
    legAnimTimer = 0;

}
    if (knifeAttacking) 

{
        knifeAnimTimer += dt;
        if (knifeAnimTimer >= 0.07f) 

{
            knifeAnimFrame++;
            knifeAnimTimer = 0;
            if (knifeAnimFrame >= KNIFE_ATTACK_FRAMES) 

{

    knifeAttacking = false;
    knifeAnimFrame = 0;

}
        }
    }
    if (currentWeaponAnim == WEAPON_KNIFE && weaponAnimState == WEAPON_ANIM_FIRE) 

{
        if (!knifeAttacking) 

{

    knifeAttacking = true;
    knifeAnimFrame = 0;
    knifeAnimTimer = 0;

}
        weaponAnimState = WEAPON_ANIM_IDLE;
    }
    Soldier::update(dt);
}

// =============================================================================
// =============================================================================

const SpriteFrame Tarma::pistolWalkFrames[14] = {
    {7,346,29,27}, {44,347,29,28}, {80,349,27,28}, {115,350,26,29},
    {149,350,28,29}, {184,349,30,28}, {222,347,29,28}, {259,345,29,28},
    {297,345,29,28}, {333,345,29,28}, {368,345,29,28}, {404,345,29,28},
    {440,345,29,30}, {478,347,30,30}
};
const SpriteFrame Tarma::legFrames[7] = {
    {10,618,21,20}, {46,618,28,20}, {84,618,31,16}, {126,618,19,20},
    {163,618,15,20}, {200,618,16,20}, {359,618,20,20}
};
const SpriteFrame Tarma::pistolFireFrames[7] = {
    {5,932,50,23}, {61,933,51,22}, {117,932,37,22}, {174,931,37,23},
    {216,929,37,25}, {258,929,37,25}, {300,931,37,23}
};
const SpriteFrame Tarma::hmgWalkFrames[5] = {
    {5,2022,37,27}, {50,2022,38,28}, {96,2022,39,28}, {144,2022,40,28}, {193,2022,39,28}
};
const SpriteFrame Tarma::hmgFireFrames[3] = {
    {8,2589,60,27}, {78,2590,36,27}, {146,2591,60,25}
};
const SpriteFrame Tarma::knifeWalkFrames[2] = {
    {472,5256,53,37}, {530,5256,51,37}
};
const SpriteFrame Tarma::knifeAttackFrames[4] = {
    {393,5197,39,46}, {442,5198,50,45}, {499,5199,53,43}, {558,5201,52,40}
};

Tarma::Tarma(float x, float y)
    : PlayerSoldier(x, y, "Tarma"), armorActive(false), armorTimer(0)


{
    grenades = getStartingGrenades();
    tarmaSheet.loadFromFile("Sprites/Tarma Roving.png");
    sprite.setTexture(tarmaSheet);
    legSprite.setTexture(tarmaSheet);
    currentWeaponAnim = WEAPON_PISTOL;
    weaponIndex = 0;
}

int Tarma::getRunFrameCount() const


{
    if (currentWeaponAnim == WEAPON_HMG)
    {
        return HMG_WALK_FRAMES;
    }
    if (currentWeaponAnim == WEAPON_KNIFE)
    {
        return KNIFE_WALK_FRAMES;
    }
    return PISTOL_WALK_FRAMES;
}

void Tarma::cycleWeapon()


{
    if (currentWeaponAnim == WEAPON_PISTOL) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = new HeavyMachineGun();
    currentWeaponAnim = WEAPON_HMG;
    weaponIndex = 1;
    }
    else if (currentWeaponAnim == WEAPON_HMG) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = new FlameShot();
    currentWeaponAnim = WEAPON_FLAME;
    weaponIndex = 3;
    }
    else if (currentWeaponAnim == WEAPON_FLAME) 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = pistol;
    currentWeaponAnim = WEAPON_KNIFE;
    weaponIndex = 2;
    }
    else 

{
        if (currentWeapon != NULL && currentWeapon != pistol) 

{

    delete currentWeapon;

}
        currentWeapon = pistol;
    currentWeaponAnim = WEAPON_PISTOL;
    weaponIndex = 0;
    }
    animFrame = 0;
    animTimer = 0;
    weaponAnimState = WEAPON_ANIM_IDLE;
    weaponAnimFrame = 0;
    weaponAnimTimer = 0;
    knifeAttacking = false;
    knifeAnimFrame = 0;
}


// Draws current frame buffer pixels to visual display

void Tarma::render(sf::RenderWindow& window) 

{

    render(window, 0, 0);

}

// Draws current frame buffer pixels to visual display
void Tarma::render(sf::RenderWindow& window, float camX, float camY)


{
    if (getTransformState() == TransformationState::mummy) 

{
        static sf::Texture mummyTex;
        static bool loaded = false;
        if (!loaded) 

{
            mummyTex.loadFromFile("Sprites/Marco Rossi (Mummy).png");
            loaded = true;
        }
        sf::Sprite mummySpr(mummyTex);
        sf::IntRect rect;
        if (knifeAttacking) 

{
            const SpriteFrame kFrames[2] = {
                {211, 462, 28, 37}, {132, 614, 28, 37}
            };
            rect = sf::IntRect(kFrames[knifeAnimFrame % 2].x, kFrames[knifeAnimFrame % 2].y, kFrames[knifeAnimFrame % 2].w, kFrames[knifeAnimFrame % 2].h);
        }
        else 

{

    rect = sf::IntRect(15, 690, 32, 28);

}
        mummySpr.setTextureRect(rect);

        float sc = 3.f;
        float drawX = x - camX;
        float drawY = y - camY;

        const SpriteFrame* legFrame = &legFrames[0];
        if (velocityX != 0)
            legFrame = &legFrames[legAnimFrame % LEG_FRAMES];

        float bodyW = 35.f * sc;
        float upperH = (float)rect.height * sc;
        float legH = (float)legFrame->h * sc;
        float legW = (float)legFrame->w * sc;
        float overlap = 6.f * sc;

        float totalH_draw = upperH + legH - overlap;
        float feetY_draw = drawY + 145.f;
        float upperY_draw = feetY_draw - totalH_draw;

        if (facingRight) 

{
            mummySpr.setScale(sc, sc);
            mummySpr.setPosition(drawX, upperY_draw);
        }
        else 

{
            mummySpr.setScale(-sc, sc);
            mummySpr.setPosition(drawX + bodyW, upperY_draw);
        }
        window.draw(mummySpr);

        float bodyCenterX = drawX + bodyW / 2.f;
        float legDrawX = bodyCenterX - legW / 2.f;
        float legY = upperY_draw + upperH - overlap;

        legSprite.setTexture(tarmaSheet);
        legSprite.setTextureRect(sf::IntRect(legFrame->x, legFrame->y, legFrame->w, legFrame->h));
        if (!facingRight) 

{
            legSprite.setScale(-sc, sc);
            legSprite.setPosition(legDrawX + legW, legY);
        }
        else 

{
            legSprite.setScale(sc, sc);
            legSprite.setPosition(legDrawX, legY);
        }
        if (!renderOnlyUpper) window.draw(legSprite);
        return;
    }

    float sc = 3.f;
    float drawX = x - camX;
    float drawY = y - camY;

    const SpriteFrame* upperFrame = NULL;
    int upperIdx = 0;

    if (currentWeaponAnim == WEAPON_KNIFE) 

{
        if (knifeAttacking) 

{
            upperIdx = knifeAnimFrame % KNIFE_ATTACK_FRAMES;
            upperFrame = &knifeAttackFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % KNIFE_WALK_FRAMES;
            upperFrame = &knifeWalkFrames[upperIdx];
        }
        else 

{

    upperFrame = &knifeWalkFrames[0];

}
    }
    else if (currentWeaponAnim == WEAPON_HMG) 

{
        if (weaponAnimState == WEAPON_ANIM_FIRE) 

{
            upperIdx = weaponAnimFrame % HMG_FIRE_FRAMES;
            upperFrame = &hmgFireFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % HMG_WALK_FRAMES;
            upperFrame = &hmgWalkFrames[upperIdx];
        }
        else 

{

    upperFrame = &hmgWalkFrames[0];

}
    }
    else 

{
        if (weaponAnimState == WEAPON_ANIM_FIRE) 

{
            upperIdx = weaponAnimFrame % PISTOL_FIRE_FRAMES;
            upperFrame = &pistolFireFrames[upperIdx];
        }
        else if (velocityX != 0) 

{
            upperIdx = animFrame % PISTOL_WALK_FRAMES;
            upperFrame = &pistolWalkFrames[upperIdx];
        }
        else 

{

    upperFrame = &pistolWalkFrames[0];

}
    }

    const SpriteFrame* legFrame = &legFrames[0];
    if (velocityX != 0)
        legFrame = &legFrames[legAnimFrame % LEG_FRAMES];

    float bodyW;
    if (currentWeaponAnim == WEAPON_KNIFE)
    {
        bodyW = 40.f * sc;
    }
    else if (currentWeaponAnim == WEAPON_HMG) bodyW = 38.f * sc;
    else bodyW = 29.f * sc;

    float upperH = (float)upperFrame->h * sc;
    float legH = (float)legFrame->h * sc;
    float legW = (float)legFrame->w * sc;

    float overlapPx;
    if (currentWeaponAnim == WEAPON_KNIFE)
    {
        overlapPx = 2.f;
    }
    else if (currentWeaponAnim == WEAPON_HMG) overlapPx = 3.f;
    else overlapPx = 6.f;
    float overlap = overlapPx * sc;

    int cropBottom;
    if (currentWeaponAnim == WEAPON_KNIFE)
    
{
    
    cropBottom = 14;
    
}
    else
    
{
    
    cropBottom = 0;
    
}
    int croppedH = upperFrame->h - cropBottom;
    float croppedUpperH = (float)croppedH * sc;

    float totalH_draw = croppedUpperH + legH - overlap;
    float feetY_draw = drawY + 145.f;
    float upperY_draw = feetY_draw - totalH_draw;

    sprite.setTexture(tarmaSheet);
    sprite.setTextureRect(sf::IntRect(upperFrame->x, upperFrame->y, upperFrame->w, croppedH));
    if (!facingRight) 

{
        sprite.setScale(-sc, sc);
        sprite.setPosition(drawX + bodyW, upperY_draw);
    }
    else 

{
        sprite.setScale(sc, sc);
        sprite.setPosition(drawX, upperY_draw);
    }
    window.draw(sprite);

    if (currentWeaponAnim == WEAPON_FLAME && weaponAnimState == WEAPON_ANIM_FIRE && !flameInterrupted) 

{
        static sf::Texture flameTex;
        static bool loaded = false;
        if (!loaded) 

{
            flameTex.loadFromFile("Sprites/Player/flame.png");
            loaded = true;
        }
        sf::Sprite flameSpr(flameTex);
        flameSpr.setScale(sc * 0.35f, sc * 0.5f);

        static float renderTimer = 0;
        static int renderFrame = 0;
        renderTimer += 0.016f;
        if (renderTimer >= 0.08f) 

{
            renderTimer = 0;
            renderFrame = (renderFrame + 1) % 8;
        }
        int frameW = flameTex.getSize().x / 8;
        if (frameW <= 0)
        {
            frameW = 32;
        }
        int frameH = flameTex.getSize().y;
        flameSpr.setTextureRect(sf::IntRect(renderFrame * frameW, 0, frameW, frameH));
        flameSpr.setOrigin(0, (float)frameH / 2.f);

        if (facingRight) 

{

    flameSpr.setPosition(drawX + bodyW - 2.f, upperY_draw + 34.f);

}
        else 

{
            flameSpr.setScale(-sc * 0.35f, sc * 0.5f);
            flameSpr.setPosition(drawX + 2.f, upperY_draw + 34.f);
        }
        window.draw(flameSpr);
    }

    upperH = croppedUpperH;
    float bodyCenterX = drawX + bodyW / 2.f;
    float legDrawX = bodyCenterX - legW / 2.f;
    float legY = upperY_draw + upperH - overlap;

    legSprite.setTexture(tarmaSheet);
    legSprite.setTextureRect(sf::IntRect(legFrame->x, legFrame->y, legFrame->w, legFrame->h));
    if (!facingRight) 

{
        legSprite.setScale(-sc, sc);
        legSprite.setPosition(legDrawX + legW, legY);
    }
    else 

{
        legSprite.setScale(sc, sc);
        legSprite.setPosition(legDrawX, legY);
    }
    if (!renderOnlyUpper) window.draw(legSprite);
}

void Tarma::activateSpecialPower()


{


    armorActive = true;
    armorTimer = 10.f;
    specialActive = true;
    specialTimer = 10.f;


}


void Tarma::update(float dt)


{
    if (getTransformState() == TransformationState::mummy) 

{
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && currentWeaponAnim == WEAPON_KNIFE) 

{
            if (!knifeAttacking) 

{

    knifeAttacking = true;
    knifeAnimFrame = 0;
    knifeAnimTimer = 0;

}
        }
        if (knifeAttacking) 

{
            knifeAnimTimer += dt;
            if (knifeAnimTimer >= 0.08f) 

{
                knifeAnimTimer = 0;
                knifeAnimFrame++;
                if (knifeAnimFrame >= 5) 

{

    knifeAttacking = false;
    knifeAnimFrame = 0;

}
            }
        }
        else if (velocityX != 0) 

{
            animTimer += dt;
            if (animTimer >= 0.15f) 

{
                animTimer = 0;
                animFrame = (animFrame + 1) % 4;
            }
        }
        else 

{

    animFrame = 0;

}
        Soldier::update(dt);
        return;
    }

    if (armorActive) 

{
        armorTimer -= dt;
        if (armorTimer <= 0) 

{

    armorActive = false;
    armorTimer = 0;

}
    }
    if (velocityX != 0 && onGround) 

{
        legAnimTimer += dt;
        if (legAnimTimer >= 0.08f) 

{
            legAnimFrame = (legAnimFrame + 1) % LEG_FRAMES;
            legAnimTimer = 0;
        }
    }
    else 

{

    legAnimFrame = 0;
    legAnimTimer = 0;

}
    if (knifeAttacking) 

{
        knifeAnimTimer += dt;
        if (knifeAnimTimer >= 0.07f) 

{
            knifeAnimFrame++;
            knifeAnimTimer = 0;
            if (knifeAnimFrame >= KNIFE_ATTACK_FRAMES) 

{

    knifeAttacking = false;
    knifeAnimFrame = 0;

}
        }
    }
    if (currentWeaponAnim == WEAPON_KNIFE && weaponAnimState == WEAPON_ANIM_FIRE) 

{
        if (!knifeAttacking) 

{

    knifeAttacking = true;
    knifeAnimFrame = 0;
    knifeAnimTimer = 0;

}
        weaponAnimState = WEAPON_ANIM_IDLE;
    }
    Soldier::update(dt);
}









 

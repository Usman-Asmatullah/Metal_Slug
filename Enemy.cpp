#include "libraries.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Soldier.h"
#include <cmath>
#include "CustomBounds.h"
#include"ScoreManager.h"
#include<iostream>
#include <cstdlib>
#include <ctime>

// Enemy base
const float Enemy::GRAVITY = 980.f;

Enemy::Enemy(float x, float y, int hp, float spd, float fr)
    : DamagableEntity(x, y, hp),
    velocityX(0), velocityY(0),
    speed(spd),
    facingRight(false),
    onGround(false),
    fireTimer(0),
    fireRate(fr),
    detectionRange(400.f),
    animTimer(0), animFrame(0),
    dying(false), deathTimer(0), deathFrame(0)


{
    this->x = x;
    this->y = y;
}

void Enemy::update(float dt, float playerX, float playerY)


{
    if (!onGround)
    {
        velocityY += GRAVITY * dt;
    }
    else if (velocityY > 0) velocityY = 0;

    move(velocityX * dt, velocityY * dt);

    if (fireTimer > 0)
    {
        fireTimer -= dt;
    }
    updateDamageState(dt);
}



void Enemy::render(sf::RenderWindow& window, float camX, float camY) 

{

}
sf::FloatRect Enemy::getBounds() const


{


    return sf::FloatRect(x, y, 40.f * 3.f, 38.f * 3.f);


}
Projectile* Enemy::shoot(float targetX, float targetY)


{


    return NULL;


}
void Enemy::moveTowardPlayer(float playerX, float dt)


{
    if (playerX < x) 

{

    velocityX = -speed;
    facingRight = false;

}
    else 

{

    velocityX = speed;
    facingRight = true;

}
}

float Enemy::angleToTarget(float targetX, float targetY) const


{


    return atan2f(targetY - y, targetX - x);


}
void Enemy::takeDamage(int amount)


{
    if (dying || DamageState == Dead)
    {
        return;
    }

    Hp -= amount;
    damageTimer = damagedStateDuration;

    if (Hp <= 0)
    

{
        Hp = 0;
        DamageState = Dead;
        dying = true;
        deathTimer = 0;
        deathFrame = 0;
        velocityX = 0;
        velocityY = 0;
    }
    else if (Hp < maxHP / 3)
    

{
    

    DamageState = criticalState;
    

}
    else if (Hp < maxHP * 2 / 3)
    

{
    

    DamageState = injuredState;
    

}
}

// RebelSoldier

const EnemyFrame RebelSoldier::walkFrames[7] = {
    {248, 44, 34, 37},   // walk 1
    {210, 44, 35, 37},   // walk 2
    {170, 45, 37, 35},   // walk 3
    {125, 46, 42, 33},   // walk 4
    { 80, 45, 42, 31},   // walk 5
    { 40, 44, 37, 37},   // walk 6
    {131, 3, 40, 38}    // walk 7
};

const EnemyFrame RebelSoldier::fireFrames[6] =
{
    {181, 444, 46, 35},  // fire 1
    {132, 444, 46, 35},  // fire 2
    { 86, 442, 43, 37},  // fire 3
    {148, 487, 41, 29},  // fire 4
    { 99, 490, 46, 26},  // fire 5
    {347, 349, 46, 38}   // fire 6 (firing/bullet frame)
};

const float RebelSoldier::WALK_FRAME_SPEED = 0.12f;
const float RebelSoldier::FIRE_FRAME_SPEED = 0.10f;
const float RebelSoldier::SCALE = 3.f;

RebelSoldier::RebelSoldier(float x, float y)
    : Enemy(x, y, 6, 80.f, 2.f), stateTimer(0), shootCooldown(2.0f),
    isFiring(false), shotFired(false), pendingShoot(false)


{
    if (!rebelSheet.loadFromFile("Sprites/Enemies/Rebel Soldier.png"))
    

{
        //  bright magenta
        sf::Image img;
        img.create(40, 40, sf::Color(255, 0, 255));
        rebelSheet.loadFromImage(img);
    }

    sprite.setTexture(rebelSheet);

   
    facingRight = (rand() % 2 == 0);
    if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
}

void RebelSoldier::update(float dt, float playerX, float playerY)


{
    if (dying)
    

{
        velocityX = 0;
        velocityY = 0;
        deathTimer += dt;
        if (deathTimer >= 1.0f)
        

{
        

    alive = false;
        

}
        return;
    }

    float dx = playerX - x;
    float absDx;
    if (dx < 0)
    
{
    
    absDx = -dx;
    
}
    else
    
{
    
    absDx = dx;
    
}
    if (isFiring)
    

{
      
        velocityX = 0;

        animTimer += dt;
        if (animTimer >= FIRE_FRAME_SPEED)
        

{
            animTimer = 0;
            animFrame++;

            if (animFrame == 5 && !shotFired)
            

{
            

    pendingShoot = true;
            

}
            if (animFrame >= FIRE_FRAMES)
            

{
                isFiring = false;
                shotFired = false;
                animFrame = 0;
                animTimer = 0;
                shootCooldown = 2.f + (float)(rand() % 2); // 2-3s cooldown
               
                if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
                stateTimer = 0;
            }
        }
    }
    else
    

{
        // Decrement shoot cooldown
        if (shootCooldown > 0)
            shootCooldown -= dt;

        if (shootCooldown <= 0 && absDx <= detectionRange)
        

{
            isFiring = true;
            shotFired = false;
            pendingShoot = false;
            animFrame = 0;
            animTimer = 0;
            // Face player
            facingRight = (dx > 0);
            velocityX = 0;
        }
        else
        

{
            stateTimer += dt;
            if (stateTimer > 3.f)
            

{
                facingRight = !facingRight;
                if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
                stateTimer = 0;
            }

            // Animate walk
            animTimer += dt;
            if (animTimer >= WALK_FRAME_SPEED)
            

{
                animTimer = 0;
                animFrame = (animFrame + 1) % WALK_FRAMES;
            }
        }
    }

    if (absDx > 2000.f)
        alive = false;

    Enemy::update(dt, playerX, playerY);
}


// Draws current frame buffer pixels to visual display

void RebelSoldier::render(sf::RenderWindow& window, float camX, float camY)


{
    if (!alive)
    {
        return;
    }
    if (dying)
    

{
        static sf::Texture deathTex;
        static bool loaded = false;
        if (!loaded)
        

{
            deathTex.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png");
            loaded = true;
        }
        sprite.setTexture(deathTex);

        const sf::IntRect deathFrames[3] = {
            {72, 1061, 28, 36},
            {32, 1061, 37, 36},
            {3, 1060, 26, 37}
        };
        int frameIdx = static_cast<int>(deathTimer / 0.15f);
        if (frameIdx > 2)
        {
            frameIdx = 2;
        }
        sf::IntRect fr = deathFrames[frameIdx];

        sprite.setTextureRect(fr);
        sprite.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>((1.0f - deathTimer) * 255)));

        float sprW = (float)fr.width * SCALE;
        float drawX = x - camX;
        float drawY = y + (38.f - (float)fr.height) * SCALE - camY;

        if (facingRight)
        

{
            sprite.setScale(-SCALE, SCALE);
            sprite.setPosition(drawX + sprW, drawY);
        }
        else
        

{
            sprite.setScale(SCALE, SCALE);
            sprite.setPosition(drawX, drawY);
        }
        window.draw(sprite);
        return;
    }
    sprite.setColor(sf::Color::White); // Color: White

   
    const EnemyFrame& fr = isFiring ? fireFrames[animFrame % FIRE_FRAMES]
        : walkFrames[animFrame % WALK_FRAMES];

    sprite.setTexture(rebelSheet);
    sprite.setTextureRect(sf::IntRect(fr.x, fr.y, fr.w, fr.h));

    float sprW = (float)fr.w * SCALE;
    float drawX = x - camX;

    float drawY = y + (38.f - (float)fr.h) * SCALE - camY;

    if (facingRight)
    

{
        sprite.setScale(-SCALE, SCALE);
        sprite.setPosition(drawX + sprW, drawY);
    }
    else
    

{
        sprite.setScale(SCALE, SCALE);
        sprite.setPosition(drawX, drawY);
    }

    window.draw(sprite);
    // draw_damage(window, sprite.getGlobalBounds());
}

sf::FloatRect RebelSoldier::getBounds() const


{
    return sf::FloatRect(x, y, 35.f * SCALE, 38.f * SCALE);
}

Projectile* RebelSoldier::shoot(float targetX, float targetY)


{
    if (pendingShoot)
    

{
        pendingShoot = false;
        shotFired = true;

        const EnemyFrame& fr = fireFrames[5]; // Shooting frame (index 5)

        float sprW = (float)fr.w * SCALE;

        float gunOffsetX = 4.f * SCALE;
        float gunOffsetY = 14.f * SCALE;

        float bulletX = 0;
        if (facingRight)
        

{
            bulletX = x + sprW - gunOffsetX;
        }
        else
        

{
            bulletX = x + gunOffsetX;
        }

        float bulletY = y + (38.f - (float)fr.h) * SCALE + gunOffsetY;

        float bulletSpeed;
        if (facingRight)
        
{
        
    bulletSpeed = 450;
        
}
        else
        
{
        
    bulletSpeed = -450;
        
}
        return new StraightProjectile(bulletX, bulletY, bulletSpeed, 0, 1, false, sf::Color::Red, 10.f, 5.f);
    }
    return NULL;
}

// =============================================================================
// =============================================================================
// =============================================================================
// =============================================================================
struct LevelSpawn {
    float x;
    int type; // 0=rebel, 1=shield, 2=bazooka, 3=grenade, 4=mummy, 5=zombie, 6=alien, 7=aerial, 8=tara, 9=bradley, 10=prisoner, 11=sub, 12=paratrooper
    bool spawned;
};

static LevelSpawn g_Level2Spawns[] = {
    { 500.f, 0, false },  // Plains - Rebel
    { 1100.f, 2, false }, // Plains - Bazooka
    { 1700.f, 12, false },// Plains - Paratrooper
    { 600.f, 4, false },  // Mummy
    { 1000.f, 8, false }, // Flying Tara

    // Restored starting tank
    { 1500.f, 9, false },

    // 3 Rocket Launchers (Subs) in Water
    { 2500.f, 11, false },
    { 3000.f, 11, false },
    { 3500.f, 11, false },

    // 2 Paratrooper waves after crossing water
    { 4100.f, 12, false },
    { 4300.f, 12, false },

    // Final M15A Bradley
    { 4500.f, 9, false }
};
static const int G_LEVEL2_SPAWN_COUNT = sizeof(g_Level2Spawns) / sizeof(g_Level2Spawns[0]);

static LevelSpawn g_Level3Spawns[] = {
    { 500.f, 0, false },
    { 800.f, 1, false },
    { 1100.f, 2, false },
    { 1400.f, 3, false },
    { 1700.f, 12, false },




    { 600.f, 4, false },
    { 1000.f, 4, false },
    { 1400.f, 4, false },

    // M15A Bradley: 2 total
    { 1500.f, 9, false },
    { 4500.f, 9, false },

    // Flying Tara
    { 1000.f, 8, false },
    { 2200.f, 8, false }
};
static const int G_LEVEL3_SPAWN_COUNT = sizeof(g_Level3Spawns) / sizeof(g_Level3Spawns[0]);

EnemyManager* GameData::activeEnemyManager = NULL;

void registerPlayerProjectile(Projectile* p)


{
    if (GameData::activeEnemyManager)
    

{
    

    GameData::activeEnemyManager->addProjectile(p);
    

}
}

EnemyManager::EnemyManager()
    : count(0), projCount(0), firstSpawnDone(false), spawnWaves(0), respawnTimer(0), m15aSpawnTimer(0), m15aSpawned(false),
    flyingTaraSpawnTimer(0), flyingTaraSpawned(false)


{
    GameData::activeEnemyManager = this;
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i] = NULL;
    }
    for (int i = 0; i < MAX_ENEMY_PROJECTILES; i++)
    {
        projectiles[i] = NULL;
    }

    for (int i = 0; i < G_LEVEL2_SPAWN_COUNT; i++)
    

{
    

    g_Level2Spawns[i].spawned = false;
    

}
    for (int i = 0; i < G_LEVEL3_SPAWN_COUNT; i++)
    

{
    

    g_Level3Spawns[i].spawned = false;
    

}
}

EnemyManager::~EnemyManager()


{
    clear();
    if (GameData::activeEnemyManager == this)
    {
        GameData::activeEnemyManager = NULL;
    }
}

EnemyManager::EnemyManager(EnemyManager&& other) noexcept
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i] = NULL;
    }
    for (int i = 0; i < MAX_ENEMY_PROJECTILES; i++)
    {
        projectiles[i] = NULL;
    }
    *this = std::move(other);
    GameData::activeEnemyManager = this; // Ensure global pointer points to the persistent instance
}

EnemyManager& EnemyManager::operator=(EnemyManager&& other) noexcept
{
    if (this != &other)
    

{
        clear();
        count = other.count;
        projCount = other.projCount;
        for (int i = 0; i < MAX_ENEMIES; i++) 

{

    enemies[i] = other.enemies[i];
    other.enemies[i] = NULL;

}
        for (int i = 0; i < MAX_ENEMY_PROJECTILES; i++) 

{

    projectiles[i] = other.projectiles[i];
    other.projectiles[i] = NULL;

}
        firstSpawnDone = other.firstSpawnDone;
        spawnWaves = other.spawnWaves;
        respawnTimer = other.respawnTimer;
        m15aSpawnTimer = other.m15aSpawnTimer;
        m15aSpawned = other.m15aSpawned;
        flyingTaraSpawnTimer = other.flyingTaraSpawnTimer;
        flyingTaraSpawned = other.flyingTaraSpawned;

        other.count = 0;
        other.projCount = 0;
        
        GameData::activeEnemyManager = this; // Ensure global pointer points to the persistent instance
    }
    return *this;
}

void EnemyManager::clear()


{
    int i = 0;
    while (i < count)
    

{
        if (enemies[i])
        

{
            delete enemies[i];
            enemies[i] = NULL;
        }
        i++;
    }

    int j = 0;
    while (j < projCount)
    

{
        if (projectiles[j])
        

{
            delete projectiles[j];
            projectiles[j] = NULL;
        }
        j++;
    }
    
    count = 0;
    projCount = 0;
}

void EnemyManager::spawnEnemy(Enemy* e)


{
    if (e)
    

{
        if (count < MAX_ENEMIES)
        

{
        

    enemies[count++] = e;
        

}
        else
        

{
        

    delete e;
        

}
    }
}

void EnemyManager::addProjectile(Projectile* p)


{
    if (p)
    

{
        if (projCount < MAX_ENEMY_PROJECTILES)
        

{
        

    projectiles[projCount++] = p;
        

}
        else
        

{
        

    delete p;
        

}
    }
}


float EnemyManager::findGroundY(int tileCol, float fallback, const World& world) const


{
    for (int row = 0; row < WORLD_H - 1; row++)
        if (world.isSolid(tileCol, row + 1))
            return (float)(row * CELL);
    return fallback;
}

void EnemyManager::update(float dt, Soldier* player, const World& world)


{
    
    if (GameData::currentLevel == 2)
    
  
     {
      updateLevel2(dt, player, world);
       return;
   }
    

    if (!player)
    {
        return;
    }
    float playerX = player->getX();
    float playerY = player->getY();


    if (firstSpawnDone && count == 0 && spawnWaves < 7)
    

{
    

    respawnTimer = 99.f;
    

}
    if (!firstSpawnDone && spawnWaves == 0)
    

{
        for (int i = 0; i < 3; i++)
        

{
            float offsetX = 200.f + (float)(rand() % 200);
            if (i % 2 == 0)
            {
                offsetX = -offsetX;
            }

            float spX = playerX + offsetX;
            int tileCol = (int)(spX / CELL);
            float spY = findGroundY(tileCol, playerY, world);
            spawnEnemy(new RebelSoldier(spX, spY));
        }
        firstSpawnDone = true;
        spawnWaves = 1;
        respawnTimer = 0;
    }

    if (spawnWaves == 1)
    

{
        respawnTimer += dt;
        if (respawnTimer >= 10.f)
        

{
            for (int i = 0; i < 3; i++)
            

{
                float offsetX = 300.f + (float)(rand() % 200);
                if (i % 2 == 0)
                {
                    offsetX = -offsetX;
                }

                float spX = playerX + offsetX;
                int tileCol = (int)(spX / CELL);
                float spY = findGroundY(tileCol, playerY, world);
                spawnEnemy(new RebelSoldier(spX, spY));
            }
            spawnWaves = 2;
            respawnTimer = 0; // Reset to count 10 seconds for Shielded Soldier Wave
        }
    }

    if (spawnWaves == 2)
    

{
        respawnTimer += dt;
        if (respawnTimer >= 10.f)
        

{
            for (int i = 0; i < 2; i++)
            

{
                float offsetX = 350.f + (float)(rand() % 150);
                if (i % 2 == 0)
                {
                    offsetX = -offsetX;
                }

                float spX = playerX + offsetX;
                int tileCol = (int)(spX / CELL);
                float spY = findGroundY(tileCol, playerY, world);
                spawnEnemy(new ShieldedSoldier(spX, spY));
            }
            spawnWaves = 3;
            respawnTimer = 0; // Reset to count 15 seconds for Bazooka Soldier Wave
        }
    }

    if (spawnWaves == 3)
    

{
        respawnTimer += dt;
        if (respawnTimer >= 10.f)
        

{
            for (int i = 0; i < 2; i++)
            

{
                float offsetX = 400.f + (float)(rand() % 150);
                if (i % 2 == 0)
                {
                    offsetX = -offsetX;
                }

                float spX = playerX + offsetX;
                int tileCol = (int)(spX / CELL);
                float spY = findGroundY(tileCol, playerY, world);
                spawnEnemy(new BazookaSoldier(spX, spY));
            }
            spawnWaves = 4; // Permanent cap at 4 waves
            respawnTimer = 0; // Reset timer for Wave 5
        }
    }

    if (spawnWaves == 4)
    

{
        respawnTimer += dt;
        if (respawnTimer >= 10.f)
        

{
            for (int i = 0; i < 2; i++)
            

{
                float offsetX = 450.f + (float)(rand() % 150);
                if (i % 2 == 0)
                {
                    offsetX = -offsetX;
                }

                float spX = playerX + offsetX;
                int tileCol = (int)(spX / CELL);
                float spY = findGroundY(tileCol, playerY, world);
                spawnEnemy(new GrenadeThrower(spX, spY));
            }
            spawnWaves = 5; // Cap at 5 waves
            respawnTimer = 0; // Reset timer for Wave 6
        }
    }

    if (spawnWaves == 5)
    

{
        respawnTimer += dt;
        if (respawnTimer >= 10.f)
        

{
            for (int i = 0; i < 3; i++)
            

{
                float offsetX = 400.f + (float)(rand() % 150) * i;
                if (i % 2 == 0)
                {
                    offsetX = -offsetX;
                }

                float spX = playerX + offsetX;
                float spY = 50.f; // Spawn high in the air!
                spawnEnemy(new Paratrooper(spX, spY));
            }
            spawnWaves = 6;
            respawnTimer = 0;
        }
    }

    if (spawnWaves == 6)
    

{
        respawnTimer += dt;
        if (respawnTimer >= 10.f)
        

{
            for (int i = 0; i < 3; i++)
            

{
                float offsetX = 300.f + (float)(rand() % 150);
                if (i % 2 == 0)
                {
                    offsetX = -offsetX;
                }

                float spX = playerX + offsetX;
                int tileCol = (int)(spX / CELL);
                float spY = findGroundY(tileCol, playerY, world);
                spawnEnemy(new MummyWarrior(spX, spY));
            }
            spawnWaves = 7;
        }
    }

    for (int i = 0; i < count; i++)
    

{
        if (!enemies[i] || !enemies[i]->isAlive() )
        {
            continue;
        }

        if (enemies[i]->getType() == Enemy::TYPE_MUMMY)
        

{
            MummyWarrior* mummy = static_cast<MummyWarrior*>(enemies[i]);
            if (!mummy->isCrumbling() && mummy->getBounds().intersects(player->getBounds()))
            

{
                if (player->getTransformState() != 2)
                {
                    player->setTransformationState(new MummyState()); // mummy state index is 2
                }
            }
        }

        enemies[i]->update(dt, playerX, playerY);
        if (enemies[i]->getType() != 7 && enemies[i]->getType() != 11)
        

{
            resolveEnemyV(*enemies[i], world);
            resolveEnemyH(*enemies[i], world);
        }

        if (enemies[i]->getType() == Enemy::TYPE_PARATROOPER && enemies[i]->isOnGround())
        

{
            float spX = enemies[i]->getX();
            float spY = enemies[i]->getY();

            delete enemies[i];

            int r = rand() % 4;
            if (r == 0)
            {
                enemies[i] = new RebelSoldier(spX, spY);
            }
            else if (r == 1) enemies[i] = new ShieldedSoldier(spX, spY);
            else if (r == 2) enemies[i] = new BazookaSoldier(spX, spY);
            else enemies[i] = new GrenadeThrower(spX, spY);
        }

        Projectile* p = enemies[i]->shoot(playerX, playerY);
        if (p)
        

{
        

    addProjectile(p);
        

}
    }

    for (int i = 0; i < projCount; i++)
        if (projectiles[i] && projectiles[i]->isAlive())
            projectiles[i]->update(dt, world, playerX, playerY);

    for (int pIdx = 0; pIdx < projCount; pIdx++)
    

{
        if (projectiles[pIdx] && projectiles[pIdx]->isAlive() && !projectiles[pIdx]->isFromPlayer() && !projectiles[pIdx]->getHasDamagedPlayer())
        

{
            sf::FloatRect projBounds = projectiles[pIdx]->getBounds();
            if (player && player->isAlive() && player->getBounds().intersects(projBounds))
            

{
                int damage = projectiles[pIdx]->getDamage();

                if (dynamic_cast<BlastProjectile*>(projectiles[pIdx]) != NULL)
                

{
                

    damage = 6;
                

}
                else if (damage > 3)
                

{
                

    damage = 3;
                

}
                player->takeDamage(damage);
                projectiles[pIdx]->setHasDamagedPlayer(true);
                projectiles[pIdx]->onHit();
            }
        }
    }

    for (int pIdx = 0; pIdx < projCount; pIdx++)
    

{
        if (projectiles[pIdx] && projectiles[pIdx]->isAlive() && projectiles[pIdx]->isFromPlayer())
        

{
            sf::FloatRect projBounds = projectiles[pIdx]->getBounds();
            for (int eIdx = 0; eIdx < count; eIdx++)
            

{
                if (enemies[eIdx] && enemies[eIdx]->isAlive())
                

{
                    if (enemies[eIdx]->getBounds().intersects(projBounds))
                    

{
                        bool blocked = false;
                        if (enemies[eIdx]->getType() == Enemy::TYPE_SHIELDED)
                        

{
                            float bulletX = projectiles[pIdx]->getX();
                            float bulletY = projectiles[pIdx]->getY();
                            bool isBehind = (enemies[eIdx]->isFacingRight() && bulletX < enemies[eIdx]->getX()) ||
                                (!enemies[eIdx]->isFacingRight() && bulletX > enemies[eIdx]->getX() + 50.f);
                            bool isAbove = (bulletY < enemies[eIdx]->getY() + 30.f);

                            if (!isBehind && !isAbove && !projectiles[pIdx]->isExplosive())
                            

{
                            

    blocked = true;
                            

}
                        }

                        if (!blocked)
                        

{
                        

    enemies[eIdx]->takeDamage(projectiles[pIdx]->getDamage());
                        

}
                        projectiles[pIdx]->onHit();
                        if (!projectiles[pIdx]->isFlamePool())
                        

{
                        

    break;
                        

}
                    }
                }
            }
        }
    }

    removeDeadEnemies();
    removeDeadProjectiles();
}


// Draws current frame buffer pixels to visual display

void EnemyManager::render(sf::RenderWindow& window, float camX, float camY)


{
    static sf::Font hpFont;
    static bool fontOK = hpFont.loadFromFile("C:/Windows/Fonts/arial.ttf");

    for (int i = 0; i < count; i++)
    

{
        if (enemies[i] && enemies[i]->isAlive())
        

{
            enemies[i]->render(window, camX, camY);

            bool isLegacyInvisible = (enemies[i]->getType() == 8 || enemies[i]->getType() == 9);
            if (fontOK && !isLegacyInvisible && enemies[i]->getHP() > 0 && !enemies[i]->isDying())
            

{
                sf::Text hpText;
                hpText.setFont(hpFont);
                hpText.setString("HP: " + std::to_string(enemies[i]->getHP()));
                hpText.setCharacterSize(14);
                hpText.setFillColor(sf::Color::Red); // Color: Red
                hpText.setStyle(sf::Text::Bold);
                hpText.setPosition(enemies[i]->getX() - camX, enemies[i]->getY() - camY - 20.f);
                window.draw(hpText);
            }
        }
    }

    for (int i = 0; i < projCount; i++)
        if (projectiles[i] && projectiles[i]->isAlive())
            projectiles[i]->render(window, camX, camY);
}

// Score Manager

class ScoreManager;

void EnemyManager::removeDeadEnemies()


{
    for (int i = 0; i < count; i++)
    

{
        if (!enemies[i]->isAlive())
        

{
            if (GameData::activeScoreManager)
            

{
            

    GameData::activeScoreManager->onEnemyKilled(enemies[i]->getType(), enemies[i]->getX(), enemies[i]->getY());
            

}
            delete enemies[i];
            enemies[i] = enemies[--count];
            enemies[count] = NULL;
            i--;
        }
    }
}

void EnemyManager::removeDeadProjectiles()


{
    for (int i = 0; i < projCount; i++)
    

{
        if (!projectiles[i]->isAlive())
        

{
            delete projectiles[i];
            projectiles[i] = projectiles[--projCount];
            projectiles[projCount] = NULL;
            i--;
        }
    }
}

void EnemyManager::resolveEnemyV(Enemy& e, const World& w)


{
    sf::FloatRect b = e.getBounds();
    int tileL = (int)(b.left / CELL);
    int tileR = (int)((b.left + b.width - 1) / CELL);
    int tileB = (int)((b.top + b.height - 1) / CELL);
    int tileT = (int)(b.top / CELL);

    e.setOnGround(false);

    if (e.getVelY() >= 0)
    

{
        for (int col = tileL; col <= tileR; col++)
        

{
            if (w.isSolid(col, tileB))
            

{
                e.setPosition(e.getX(), (float)(tileB * CELL) - b.height);
                e.setVelY(0);
                e.setOnGround(true);
                break;
            }
        }
    }
    else
    

{
        for (int col = tileL; col <= tileR; col++)
        

{
            if (w.isSolid(col, tileT))
            

{
                e.setPosition(e.getX(), (float)((tileT + 1) * CELL));
                e.setVelY(0);
                break;
            }
        }
    }
}

void EnemyManager::resolveEnemyH(Enemy& e, const World& w)


{
    sf::FloatRect b = e.getBounds();
    int tileL = (int)(b.left / CELL);
    int tileR = (int)((b.left + b.width - 1) / CELL);
    int tileT = (int)(b.top / CELL);
    int tileB = (int)((b.top + b.height - 1) / CELL);

    for (int row = tileT; row <= tileB; row++)
    

{
        if (e.getVelX() < 0 && w.isSolid(tileL, row))
        

{
            e.setPosition((float)((tileL + 1) * CELL), e.getY());
            e.setVelX(0);
            break;
        }
        if (e.getVelX() > 0 && w.isSolid(tileR, row))
        

{
            e.setPosition((float)(tileR * CELL) - b.width, e.getY());
            e.setVelX(0);
            break;
        }
    }
}

// ShieldedSoldier


const EnemyFrame ShieldedSoldier::walkFrames[6] = {
    {359, 48, 27, 39},   // walk 1
    {329, 49, 27, 38},   // walk 2
    {298, 47, 28, 39},   // walk 3
    {235, 46, 29, 41},   // walk 4
    {202, 47, 30, 40},   // walk 5
    {167, 48, 32, 39}    // walk 6
};

const EnemyFrame ShieldedSoldier::fireFrames[10] = {
    { 3, 305, 30, 38},  // fire 1
    { 36, 305, 28, 38},  // fire 2
    { 67, 305, 31, 38},  // fire 3
    {101, 305, 31, 38},  // fire 4
    {135, 305, 38, 38},  // fire 5
    {176, 305, 37, 38},  // fire 6
    {216, 301, 35, 42},  // fire 7
    {254, 305, 34, 38},  // fire 8
    {291, 305, 35, 38},  // fire 9
    {329, 302, 37, 41}   // fire 10 (firing/bullet frame)
};

const float ShieldedSoldier::WALK_FRAME_SPEED = 0.12f;
const float ShieldedSoldier::FIRE_FRAME_SPEED = 0.10f;
const float ShieldedSoldier::SCALE = 3.f;

ShieldedSoldier::ShieldedSoldier(float x, float y)
    : Enemy(x, y, 12, 60.f, 2.f), stateTimer(0), shootCooldown(2.0f),
    isFiring(false), shotFired(false), pendingShoot(false), shieldActive(true)


{
    if (!texture.loadFromFile("Sprites/Enemies/Rebel Soldier (Shield).png"))
    

{
        // Fallback: green texture
        sf::Image img;
        img.create(40, 40, sf::Color(0, 255, 0));
        texture.loadFromImage(img);
    }

    sprite.setTexture(texture);

    // Random initial direction
    facingRight = (rand() % 2 == 0);
    if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
}

void ShieldedSoldier::update(float dt, float playerX, float playerY)


{
    if (dying)
    

{
        velocityX = 0;
        velocityY = 0;
        shieldActive = false;
        deathTimer += dt;
        if (deathTimer >= 1.0f)
        

{
        

    alive = false;
        

}
        return;
    }

    float dx = playerX - x;
    float absDx;
    if (dx < 0)
    
{
    
    absDx = -dx;
    
}
    else
    
{
    
    absDx = dx;
    
}
    if (isFiring)
    

{
        // Stand still while firing
        velocityX = 0;
        shieldActive = false; // Shield removed while firing!

        animTimer += dt;
        if (animTimer >= FIRE_FRAME_SPEED)
        

{
            animTimer = 0;
            animFrame++;

            if (animFrame == 9 && !shotFired)
            

{
            

    pendingShoot = true;
            

}
            if (animFrame >= FIRE_FRAMES)
            

{
                isFiring = false;
                shotFired = false;
                animFrame = 0;
                animTimer = 0;
                shootCooldown = 2.f + (float)(rand() % 2); // 2-3s cooldown
                // Resume patrolling
                if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
                stateTimer = 0;
                shieldActive = true; // Shield reactivated!
            }
        }
    }
    else
    

{
        shieldActive = true; // Shield is active while moving/patrolling

        // Decrement shoot cooldown
        if (shootCooldown > 0)
            shootCooldown -= dt;

        if (shootCooldown <= 0 && absDx <= detectionRange)
        

{
            isFiring = true;
            shotFired = false;
            pendingShoot = false;
            animFrame = 0;
            animTimer = 0;
            // Face player
            facingRight = (dx > 0);
            velocityX = 0;
            shieldActive = false;
        }
        else
        

{
            stateTimer += dt;
            if (stateTimer > 3.f)
            

{
                facingRight = !facingRight;
                if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
                stateTimer = 0;
            }

            // Animate walk
            animTimer += dt;
            if (animTimer >= WALK_FRAME_SPEED)
            

{
                animTimer = 0;
                animFrame = (animFrame + 1) % WALK_FRAMES;
            }
        }
    }

    if (absDx > 2000.f)
        alive = false;

    Enemy::update(dt, playerX, playerY);
}


// Draws current frame buffer pixels to visual display

void ShieldedSoldier::render(sf::RenderWindow& window, float camX, float camY)


{
    if (!alive)
    {
        return;
    }
    if (dying)
    

{
        static sf::Texture deathTex;
        static bool loaded = false;
        if (!loaded)
        

{
            deathTex.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png");
            loaded = true;
        }
        sprite.setTexture(deathTex);

        const sf::IntRect deathFrames[3] = {
            {72, 1061, 28, 36},
            {32, 1061, 37, 36},
            {3, 1060, 26, 37}
        };
        int frameIdx = static_cast<int>(deathTimer / 0.15f);
        if (frameIdx > 2)
        {
            frameIdx = 2;
        }
        sf::IntRect fr = deathFrames[frameIdx];

        sprite.setTextureRect(fr);
        sprite.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>((1.0f - deathTimer) * 255)));

        float sprW = (float)fr.width * SCALE;
        float drawX = x - camX;
        float drawY = y + (42.f - (float)fr.height) * SCALE - camY;

        if (facingRight)
        

{
            sprite.setScale(-SCALE, SCALE);
            sprite.setPosition(drawX + sprW, drawY);
        }
        else
        

{
            sprite.setScale(SCALE, SCALE);
            sprite.setPosition(drawX, drawY);
        }
        window.draw(sprite);
        return;
    }
    sprite.setColor(sf::Color::White); // Color: White

  
    const EnemyFrame& fr = isFiring ? fireFrames[animFrame % FIRE_FRAMES]
        : walkFrames[animFrame % WALK_FRAMES];

    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(fr.x, fr.y, fr.w, fr.h));

    float sprW = (float)fr.w * SCALE;
    float drawX = x - camX;

    float drawY = y + (42.f - (float)fr.h) * SCALE - camY;

    if (facingRight)
    

{
        sprite.setScale(-SCALE, SCALE);
        sprite.setPosition(drawX + sprW, drawY);
    }
    else
    

{
        sprite.setScale(SCALE, SCALE);
        sprite.setPosition(drawX, drawY);
    }

    window.draw(sprite);
    // draw_damage(window, sprite.getGlobalBounds());
}

sf::FloatRect ShieldedSoldier::getBounds() const


{
    return sf::FloatRect(x, y, 32.f * SCALE, 42.f * SCALE);
}

bool ShieldedSoldier::isHitBlocked(float bulletX) const


{
    bool blocked = false;
    if (facingRight && bulletX > x)
    {
        blocked = true;
    }
    if (!facingRight && bulletX < x)
    {
        blocked = true;
    }

    return blocked;
}

void ShieldedSoldier::takeDamage(int amount)


{
    int realDamage;
    if (amount < 0)
    
{
    
    realDamage = -amount;
    
}
    else
    
{
    
    realDamage = amount;
    
}
    Enemy::takeDamage(realDamage);
}

Projectile* ShieldedSoldier::shoot(float targetX, float targetY)


{
    if (pendingShoot)
    

{
        pendingShoot = false;
        shotFired = true;

        const EnemyFrame& fr = fireFrames[9]; // Shooting frame (index 9)

        float sprW = (float)fr.w * SCALE;

        float gunOffsetX = 4.f * SCALE;
        float gunOffsetY = 14.f * SCALE;

        float bulletX = 0;
        if (facingRight)
        

{
            bulletX = x + sprW - gunOffsetX;
        }
        else
        

{
            bulletX = x + gunOffsetX;
        }

        float bulletY = y + (42.f - (float)fr.h) * SCALE + gunOffsetY;

        float bulletSpeed;
        if (facingRight)
        
{
        
    bulletSpeed = 450;
        
}
        else
        
{
        
    bulletSpeed = -450;
        
}
        return new StraightProjectile(bulletX, bulletY, bulletSpeed, 0, 1, false, sf::Color::Red, 10.f, 5.f);
    }
    return NULL;
}


// BazookaSoldier Implementation

const EnemyFrame BazookaSoldier::spawnFrames[7] = {
    { 3, 953, 55, 43}, // parachute spawn 1
    { 61, 955, 54, 44}, // parachute spawn 2
    {118, 959, 56, 43}, // parachute spawn 3
    {177, 963, 55, 47}, // parachute spawn 4
    {235, 968, 53, 44}, // parachute spawn 5
    {291, 975, 54, 40}, // parachute spawn 6
    {348, 986, 56, 34}  // parachute spawn 7
};

const EnemyFrame BazookaSoldier::hangingFrames[2] = {
    { 3, 1060, 26, 37}, // frame 1
    { 32, 1061, 37, 36}  // frame 2
};

const EnemyFrame BazookaSoldier::walkFrames[6] = {
    {124, 6, 36, 42}, // walk 1
    {221, 55, 41, 39}, // walk 2
    {178, 56, 40, 38}, // walk 3
    {134, 54, 41, 39}, // walk 4
    {429, 53, 37, 41}, // walk 5
    {204, 3, 38, 45}  // walk 6
};

const EnemyFrame BazookaSoldier::fireFrames[3] = {
    { 3, 309, 43, 37}, // fire 1
    { 49, 302, 40, 44}, // fire 2
    { 92, 302, 39, 44}  // fire 3
};

const float BazookaSoldier::WALK_FRAME_SPEED = 0.12f;
const float BazookaSoldier::FIRE_FRAME_SPEED = 0.15f;
const float BazookaSoldier::SPAWN_FRAME_SPEED = 0.10f;
const float BazookaSoldier::SCALE = 3.f;

BazookaSoldier::BazookaSoldier(float x, float y)
    : Enemy(x, y, 9, 50.f, 1.5f), stateTimer(0), shootCooldown(3.0f),
    isFiring(false), shotFired(false), pendingShoot(false),
    isDescending(false), descendSpeed(80.f)


{
    texture.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png");
    sprite.setTexture(texture);
    facingRight = (rand() % 2 == 0);
    if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
    velocityY = 0;
}

void BazookaSoldier::update(float dt, float playerX, float playerY)


{
    if (dying)
    

{
        velocityX = 0;
        velocityY = 0;
        deathTimer += dt;
        if (deathTimer >= 1.0f)
        

{
        

    alive = false;
        

}
        return;
    }

    float dx = playerX - x;
    float absDx;
    if (dx < 0)
    
{
    
    absDx = -dx;
    
}
    else
    
{
    
    absDx = dx;
    
}
    if (isDescending)
    

{
        if (onGround)
        

{
            isDescending = false;
            velocityY = 0;
            if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
            animFrame = 0;
            animTimer = 0;
        }
        else
        

{
            // Parachute float loop
            animTimer += dt;
            if (animTimer >= SPAWN_FRAME_SPEED)
            

{
                animTimer = 0;
                animFrame = (animFrame + 1) % SPAWN_FRAMES;
            }
            velocityY = descendSpeed;
            velocityX = 0;
        }
    }
    else if (isFiring)
    

{
        velocityX = 0;

        animTimer += dt;
        if (animTimer >= FIRE_FRAME_SPEED)
        

{
            animTimer = 0;
            animFrame++;

            if (animFrame == 2 && !shotFired)
            

{
            

    pendingShoot = true;
            

}
            if (animFrame >= FIRE_FRAMES)
            

{
                isFiring = false;
                shotFired = false;
                animFrame = 0;
                animTimer = 0;
                shootCooldown = 2.5f + (float)(rand() % 2);
                if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
            }
        }
    }
    else
    

{
        if (shootCooldown > 0)
            shootCooldown -= dt;

        if (shootCooldown <= 0 && absDx <= detectionRange)
        

{
            isFiring = true;
            shotFired = false;
            pendingShoot = false;
            animFrame = 0;
            animTimer = 0;
            facingRight = (dx > 0);
            velocityX = 0;
        }
        else
        

{
            stateTimer += dt;
            if (stateTimer > 3.f)
            

{
                facingRight = !facingRight;
                if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
                stateTimer = 0;
            }

            animTimer += dt;
            if (animTimer >= WALK_FRAME_SPEED)
            

{
                animTimer = 0;
                animFrame = (animFrame + 1) % WALK_FRAMES;
            }
        }
    }

    if (absDx > 2000.f)
        alive = false;

    if (isDescending)
    

{
    

    move(velocityX * dt, velocityY * dt);
    

}
    else
    

{
    

    Enemy::update(dt, playerX, playerY);
    

}
}


// Draws current frame buffer pixels to visual display

void BazookaSoldier::render(sf::RenderWindow& window, float camX, float camY)


{
    if (!alive)
    {
        return;
    }
    if (dying)
    

{
        static sf::Texture deathTex;
        static bool loaded = false;
        if (!loaded)
        

{
            deathTex.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png");
            loaded = true;
        }
        sprite.setTexture(deathTex);

        const sf::IntRect deathFrames[3] = {
            {72, 1061, 28, 36},
            {32, 1061, 37, 36},
            {3, 1060, 26, 37}
        };
        int frameIdx = static_cast<int>(deathTimer / 0.15f);
        if (frameIdx > 2)
        {
            frameIdx = 2;
        }
        sf::IntRect fr = deathFrames[frameIdx];

        sprite.setTextureRect(fr);
        sprite.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>((1.0f - deathTimer) * 255)));

        float sprW = (float)fr.width * SCALE;
        float drawX = x - camX;
        float drawY = y + (45.f - (float)fr.height) * SCALE - camY;

        if (facingRight)
        

{
            sprite.setScale(-SCALE, SCALE);
            sprite.setPosition(drawX + sprW, drawY);
        }
        else
        

{
            sprite.setScale(SCALE, SCALE);
            sprite.setPosition(drawX, drawY);
        }
        window.draw(sprite);
        return;
    }
    sprite.setColor(sf::Color::White); // Color: White

    if (isDescending)
    

{
        const EnemyFrame& paraFr = spawnFrames[animFrame % SPAWN_FRAMES];
        const EnemyFrame& soldFr = hangingFrames[animFrame % HANGING_FRAMES];
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(paraFr.x, paraFr.y, paraFr.w, paraFr.h));

        float sprParaW = (float)paraFr.w * SCALE;
        float drawX = x - camX;
        float drawY = y + (74.f - ((float)paraFr.h + (float)soldFr.h - 5.f)) * SCALE - camY;

        if (facingRight)
        

{
            sprite.setScale(-SCALE, SCALE);
            sprite.setPosition(drawX + sprParaW, drawY);
        }
        else
        

{
            sprite.setScale(SCALE, SCALE);
            sprite.setPosition(drawX, drawY);
        }
        window.draw(sprite);

        sf::Sprite soldierSprite(texture);
        soldierSprite.setTextureRect(sf::IntRect(soldFr.x, soldFr.y, soldFr.w, soldFr.h));

        float sprSoldW = (float)soldFr.w * SCALE;
        float soldDrawX = drawX + (sprParaW - sprSoldW) / 2.f;
        if (facingRight)
        

{
        

    soldDrawX = drawX + sprParaW - (sprParaW - sprSoldW) / 2.f;
        

}
        float soldDrawY = drawY + (float)paraFr.h * SCALE - 5.f * SCALE;

        if (facingRight)
        

{
            soldierSprite.setScale(-SCALE, SCALE);
            soldierSprite.setPosition(soldDrawX, soldDrawY);
        }
        else
        

{
            soldierSprite.setScale(SCALE, SCALE);
            soldierSprite.setPosition(soldDrawX, soldDrawY);
        }
        window.draw(soldierSprite);
        // draw_damage(window, soldierSprite.getGlobalBounds());
    }
    else
    

{
        const EnemyFrame& fr = isFiring ? fireFrames[animFrame % FIRE_FRAMES]
            : walkFrames[animFrame % WALK_FRAMES];

        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(fr.x, fr.y, fr.w, fr.h));

        float sprW = (float)fr.w * SCALE;
        float drawX = x - camX;
        float drawY = y + (45.f - (float)fr.h) * SCALE - camY;

        if (facingRight)
        

{
            sprite.setScale(-SCALE, SCALE);
            sprite.setPosition(drawX + sprW, drawY);
        }
        else
        

{
            sprite.setScale(SCALE, SCALE);
            sprite.setPosition(drawX, drawY);
        }

        window.draw(sprite);
        // draw_damage(window, sprite.getGlobalBounds());
    }
}

sf::FloatRect BazookaSoldier::getBounds() const


{
    float stableW;
    if (isDescending)
    
{
    
    stableW = 55;
    
}
    else
    
{
    
    stableW = 38;
    
}
    float stableH;
    if (isDescending)
    
{
    
    stableH = 74;
    
}
    else
    
{
    
    stableH = 45;
    
}
    return sf::FloatRect(x, y, stableW * SCALE, stableH * SCALE);
}

Projectile* BazookaSoldier::shoot(float targetX, float targetY)


{
    if (pendingShoot)
    

{
        pendingShoot = false;
        shotFired = true;

        const EnemyFrame& fr = fireFrames[2]; // frame 3

        float sprW = (float)fr.w * SCALE;

        float gunOffsetX = 4.f * SCALE;
        float gunOffsetY = 14.f * SCALE;

        float bulletX = 0;
        if (facingRight)
        

{
        

    bulletX = x + sprW - gunOffsetX;
        

}
        else
        

{
        

    bulletX = x + gunOffsetX;
        

}
        float bulletY = y + (45.f - (float)fr.h) * SCALE + gunOffsetY;

        float bulletSpeed;
        if (facingRight)
        
{
        
    bulletSpeed = 400;
        
}
        else
        
{
        
    bulletSpeed = -400;
        
}
        return new BombProjectile(bulletX, bulletY, bulletSpeed, 0, 2, false);
    }
    return NULL;
}

// GrenadeThrower Implementation
const float GrenadeThrower::WALK_FRAME_SPEED = 0.15f;
const float GrenadeThrower::FIRE_FRAME_SPEED = 0.12f;
const float GrenadeThrower::SCALE = 3.f;

const EnemyFrame GrenadeThrower::walkFrames[1] = {
    {307, 136, 37, 37}
};

const EnemyFrame GrenadeThrower::fireFrames[8] = {
    {307, 136, 37, 37}, // frame 1
    {274, 128, 30, 45}, // frame 2
    {227, 126, 44, 47}, // frame 3
    {183, 125, 41, 37}, // frame 4
    {144, 126, 36, 36}, // frame 5
    { 99, 129, 42, 36}, // frame 6
    { 49, 130, 47, 43}, // frame 7
    { 3, 136, 43, 37}  // frame 8
};

GrenadeThrower::GrenadeThrower(float x, float y)
    : Enemy(x, y, 9, 50.f, 2.0f), stateTimer(0), shootCooldown(3.0f),
    isFiring(false), shotFired(false), pendingShoot(false)


{
    texture.loadFromFile("Sprites/Enemies/Rebel Soldier.png");
    sprite.setTexture(texture);
    facingRight = (rand() % 2 == 0);
    if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
    velocityY = 0;
}

void GrenadeThrower::update(float dt, float playerX, float playerY)


{
    if (dying)
    

{
        velocityX = 0;
        velocityY = 0;
        deathTimer += dt;
        if (deathTimer >= 1.0f)
        

{
        

    alive = false;
        

}
        return;
    }

    float dx = playerX - x;
    float absDx;
    if (dx < 0)
    
{
    
    absDx = -dx;
    
}
    else
    
{
    
    absDx = dx;
    
}
    if (isFiring)
    

{
        velocityX = 0;

        animTimer += dt;
        if (animTimer >= FIRE_FRAME_SPEED)
        

{
            animTimer = 0;
            animFrame++;

            if (animFrame == 2 && !shotFired)
            

{
            

    pendingShoot = true;
            

}
            if (animFrame >= FIRE_FRAMES)
            

{
                isFiring = false;
                shotFired = false;
                animFrame = 0;
                animTimer = 0;
                shootCooldown = 2.0f + (float)(rand() % 2);
                if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
            }
        }
    }
    else
    

{
        if (shootCooldown > 0)
            shootCooldown -= dt;

        if (shootCooldown <= 0 && absDx <= detectionRange)
        

{
            isFiring = true;
            shotFired = false;
            pendingShoot = false;
            animFrame = 0;
            animTimer = 0;
            facingRight = (dx > 0);
            velocityX = 0;
        }
        else
        

{
            stateTimer += dt;
            if (stateTimer > 3.f)
            

{
                facingRight = !facingRight;
                if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
                stateTimer = 0;
            }

            animTimer += dt;
            if (animTimer >= WALK_FRAME_SPEED)
            

{
                animTimer = 0;
                animFrame = (animFrame + 1) % WALK_FRAMES;
            }
        }
    }

    if (absDx > 2000.f)
        alive = false;

    Enemy::update(dt, playerX, playerY);
}


// Draws current frame buffer pixels to visual display

void GrenadeThrower::render(sf::RenderWindow& window, float camX, float camY)


{
    if (!alive)
    {
        return;
    }
    if (dying)
    

{
        static sf::Texture deathTex;
        static bool loaded = false;
        if (!loaded)
        

{
            deathTex.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png");
            loaded = true;
        }
        sprite.setTexture(deathTex);

        const sf::IntRect deathFrames[3] = {
            {72, 1061, 28, 36},
            {32, 1061, 37, 36},
            {3, 1060, 26, 37}
        };
        int frameIdx = static_cast<int>(deathTimer / 0.15f);
        if (frameIdx > 2)
        {
            frameIdx = 2;
        }
        sf::IntRect fr = deathFrames[frameIdx];

        sprite.setTextureRect(fr);
        sprite.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>((1.0f - deathTimer) * 255)));

        float sprW = (float)fr.width * SCALE;
        float drawX = x - camX;
        float drawY = y + (45.f - (float)fr.height) * SCALE - camY;

        if (facingRight)
        

{
            sprite.setScale(-SCALE, SCALE);
            sprite.setPosition(drawX + sprW, drawY);
        }
        else
        

{
            sprite.setScale(SCALE, SCALE);
            sprite.setPosition(drawX, drawY);
        }
        window.draw(sprite);
        return;
    }
    sprite.setColor(sf::Color::White); // Color: White

    const EnemyFrame& fr = isFiring ? fireFrames[animFrame % FIRE_FRAMES]
        : walkFrames[animFrame % WALK_FRAMES];

    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(fr.x, fr.y, fr.w, fr.h));

    float sprW = (float)fr.w * SCALE;
    float drawX = x - camX;

    float drawY = y + (45.f - (float)fr.h) * SCALE - camY;

    if (facingRight)
    

{
        sprite.setScale(-SCALE, SCALE);
        sprite.setPosition(drawX + sprW, drawY);
    }
    else
    

{
        sprite.setScale(SCALE, SCALE);
        sprite.setPosition(drawX, drawY);
    }

    window.draw(sprite);
    // draw_damage(window, sprite.getGlobalBounds());
}

sf::FloatRect GrenadeThrower::getBounds() const


{


    return sf::FloatRect(x, y, 37.f * SCALE, 45.f * SCALE);


}
Projectile* GrenadeThrower::shoot(float targetX, float targetY)


{
    if (pendingShoot)
    

{
        pendingShoot = false;
        shotFired = true;

        const EnemyFrame& fr = fireFrames[2]; // frame 3

        float sprW = (float)fr.w * SCALE;

        float handOffsetX = 12.f * SCALE;
        float handOffsetY = 10.f * SCALE;

        float bulletX = 0;
        if (facingRight)
        

{
        

    bulletX = x + sprW - handOffsetX;
        

}
        else
        

{
        

    bulletX = x + handOffsetX;
        

}
        float bulletY = y + (45.f - (float)fr.h) * SCALE + handOffsetY;

        float dx = targetX - bulletX;
        float bulletSpeedX;
        if (dx > 0)
        
{
        
    bulletSpeedX = 280;
        
}
        else
        
{
        
    bulletSpeedX = -280;
        
}
        float bulletSpeedY = -150.f; // Flatter and more direct throw arc towards the player

        return new GrenadeProjectile(bulletX, bulletY, bulletSpeedX, bulletSpeedY, 2, false);
    }
    return NULL;
}

// Paratrooper Implementation
const float Paratrooper::SPAWN_FRAME_SPEED = 0.15f;
const float Paratrooper::SCALE = 3.f;

const EnemyFrame Paratrooper::spawnFrames[7] = {
    {3, 953, 55, 43},
    {61, 955, 54, 44},
    {118, 959, 56, 43},
    {177, 963, 55, 47},
    {235, 968, 53, 44},
    {291, 975, 54, 40},
    {348, 986, 56, 34}
};

const EnemyFrame Paratrooper::hangingFrames[2] = {
    { 3, 1060, 26, 37}, // frame 1
    { 32, 1061, 37, 36}  // frame 2
};

Paratrooper::Paratrooper(float x, float y)
    : Enemy(x, y, 9, 50.f, 1.0f), isDescending(true), descendSpeed(80.f)


{
    texture.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png");
    sprite.setTexture(texture);
    facingRight = (rand() % 2 == 0);
    velocityX = 0;
    velocityY = descendSpeed;
}

void Paratrooper::update(float dt, float playerX, float playerY)


{
    if (dying)
    

{
        velocityX = 0;
        velocityY = 0;
        deathTimer += dt;
        if (deathTimer >= 1.0f)
        

{
        

    alive = false;
        

}
        return;
    }

    // Parachute floating animation loop
    animTimer += dt;
    if (animTimer >= SPAWN_FRAME_SPEED)
    

{
        animTimer = 0;
        animFrame = (animFrame + 1) % SPAWN_FRAMES;
    }
    velocityY = descendSpeed;
    velocityX = 0;

    move(0, velocityY * dt);
}


// Draws current frame buffer pixels to visual display

void Paratrooper::render(sf::RenderWindow& window, float camX, float camY)


{
    if (!alive)
    {
        return;
    }
    if (dying)
    

{
        static sf::Texture deathTex;
        static bool loaded = false;
        if (!loaded)
        

{
            deathTex.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png");
            loaded = true;
        }
        sprite.setTexture(deathTex);

        const sf::IntRect deathFrames[3] = {
            {72, 1061, 28, 36},
            {32, 1061, 37, 36},
            {3, 1060, 26, 37}
        };
        int frameIdx = static_cast<int>(deathTimer / 0.15f);
        if (frameIdx > 2)
        {
            frameIdx = 2;
        }
        sf::IntRect fr = deathFrames[frameIdx];

        sprite.setTextureRect(fr);
        sprite.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>((1.0f - deathTimer) * 255)));

        float sprW = (float)fr.width * SCALE;
        float drawX = x - camX;
        float drawY = y + (74.f - (float)fr.height) * SCALE - camY;

        if (facingRight)
        

{
            sprite.setScale(-SCALE, SCALE);
            sprite.setPosition(drawX + sprW, drawY);
        }
        else
        

{
            sprite.setScale(SCALE, SCALE);
            sprite.setPosition(drawX, drawY);
        }
        window.draw(sprite);
        return;
    }
    sprite.setColor(sf::Color::White); // Color: White

    // Render the parachute
    const EnemyFrame& paraFr = spawnFrames[animFrame % SPAWN_FRAMES];
    const EnemyFrame& soldFr = hangingFrames[animFrame % HANGING_FRAMES];
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(paraFr.x, paraFr.y, paraFr.w, paraFr.h));

    float sprParaW = (float)paraFr.w * SCALE;
    float drawX = x - camX;

    float drawY = y + (74.f - ((float)paraFr.h + (float)soldFr.h - 5.f)) * SCALE - camY;

    if (facingRight)
    

{
        sprite.setScale(-SCALE, SCALE);
        sprite.setPosition(drawX + sprParaW, drawY);
    }
    else
    

{
        sprite.setScale(SCALE, SCALE);
        sprite.setPosition(drawX, drawY);
    }
    window.draw(sprite);

    sf::Sprite soldierSprite(texture);
    soldierSprite.setTextureRect(sf::IntRect(soldFr.x, soldFr.y, soldFr.w, soldFr.h));

    float sprSoldW = (float)soldFr.w * SCALE;
    float soldDrawX = drawX + (sprParaW - sprSoldW) / 2.f;
    if (facingRight)
    

{
    

    soldDrawX = drawX + sprParaW - (sprParaW - sprSoldW) / 2.f;
    

}
    float soldDrawY = drawY + (float)paraFr.h * SCALE - 5.f * SCALE;

    if (facingRight)
    

{
        soldierSprite.setScale(-SCALE, SCALE);
        soldierSprite.setPosition(soldDrawX, soldDrawY);
    }
    else
    

{
        soldierSprite.setScale(SCALE, SCALE);
        soldierSprite.setPosition(soldDrawX, soldDrawY);
    }
    window.draw(soldierSprite);
    // draw_damage(window, soldierSprite.getGlobalBounds());
}

sf::FloatRect Paratrooper::getBounds() const


{


    return sf::FloatRect(x, y, 55.f * SCALE, 74.f * SCALE);


}

// MUMMY WARRIOR IMPLEMENTATION

const float MummyWarrior::WALK_FRAME_SPEED = 0.12f;
const float MummyWarrior::CRUMBLE_FRAME_SPEED = 0.15f;
const float MummyWarrior::SCALE = 3.f;

const EnemyFrame MummyWarrior::walkFrames[8] = {
    {195, 1010, 31, 45}, {235, 1010, 31, 45}, {275, 1010, 31, 45}, {314, 1010, 33, 45},
    {355, 1011, 31, 44}, {471, 1010, 31, 45}, {593, 1010, 31, 45}, {508, 1010, 32, 45}
};

const EnemyFrame MummyWarrior::crumbleFrames[7] = {
    {1036, 1264, 31, 43}, {989, 1263, 31, 44}, {854, 1265, 33, 42}, {711, 1252, 36, 55},
    {455, 1253, 41, 54}, {253, 1259, 39, 48}, {106, 1261, 39, 46}
};

const EnemyFrame MummyWarrior::deathFrames[11] = {
    {10, 1336, 29, 42}, {46, 1337, 29, 41}, {82, 1337, 29, 41}, {193, 1341, 29, 37},
    {270, 1348, 30, 30}, {383, 1361, 30, 17}, {501, 1330, 28, 48}, {537, 1324, 25, 54},
    {571, 1321, 22, 57}, {601, 1322, 16, 56}, {627, 1338, 10, 40}
};

MummyWarrior::MummyWarrior(float x, float y)
    : Enemy(x, y, 5, 50.f, 0), stateTimer(0), crumbling(false), crumbleTimer(0)


{
    texture.loadFromFile("Sprites/Enemies/Enemy Mummy.png");
    sprite.setTexture(texture);
    onGround = true;
}

void MummyWarrior::update(float dt, float playerX, float playerY)


{
    updateDamageState(dt);

    if (dying)
    

{
        velocityX = 0;
        velocityY = 0;
        deathTimer += dt;
        animTimer += dt;
        if (animTimer >= 0.12f)
        

{
            animTimer = 0;
            if (deathFrame < DEATH_FRAMES - 1)
            

{
            

    deathFrame++;
            

}
            else
            

{
            

    alive = false;
            

}
        }
        return;
    }

    if (crumbling)
    

{
        velocityX = 0;
        velocityY = 0;
        crumbleTimer -= dt;
        animTimer += dt;
        if (animTimer >= CRUMBLE_FRAME_SPEED)
        

{
            animTimer = 0;
            if (animFrame < CRUMBLE_FRAMES - 1)
            

{
            

    animFrame++;
            

}
        }

        if (crumbleTimer <= 0)
        

{
            crumbling = false;
            Hp = maxHP;
            DamageState = normalState;
            animFrame = 0;
            animTimer = 0;
        }
        return;
    }

    velocityY += GRAVITY * dt;
    if (playerX < x)
    

{
        velocityX = -speed;
        facingRight = false;
    }
    else
    

{
        velocityX = speed;
        facingRight = true;
    }

    x += velocityX * dt;
    y += velocityY * dt;

    animTimer += dt;
    if (animTimer >= WALK_FRAME_SPEED)
    

{
        animTimer = 0;
        animFrame = (animFrame + 1) % WALK_FRAMES;
    }
}




void MummyWarrior::render(sf::RenderWindow& window, float camX, float camY)


{
    if (!alive)
    {
        return;
    }

    if (dying)
    

{
        EnemyFrame fr = deathFrames[deathFrame % DEATH_FRAMES];
        sprite.setTextureRect(sf::IntRect(fr.x, fr.y, fr.w, fr.h));

        float drawX = x - camX;
        float drawY = y - camY;

        if (facingRight)
        

{
            sprite.setScale(-SCALE, SCALE);
            sprite.setPosition(drawX + (float)fr.w * SCALE, drawY);
        }
        else
        

{
            sprite.setScale(SCALE, SCALE);
            sprite.setPosition(drawX, drawY);
        }

        window.draw(sprite);
        return;
    }

    EnemyFrame fr;
    if (crumbling)
    
{
    
    fr = crumbleFrames[animFrame % CRUMBLE_FRAMES];
    
}
    else
    
{
    
    fr = walkFrames[animFrame % WALK_FRAMES];
    
}
    sprite.setTextureRect(sf::IntRect(fr.x, fr.y, fr.w, fr.h));

    float drawX = x - camX;
    float drawY = y - camY;

    if (facingRight)
    

{
        sprite.setScale(-SCALE, SCALE);
        sprite.setPosition(drawX + (float)fr.w * SCALE, drawY);
    }
    else
    

{
        sprite.setScale(SCALE, SCALE);
        sprite.setPosition(drawX, drawY);
    }

    window.draw(sprite);
    // draw_damage(window, sprite.getGlobalBounds());
}

sf::FloatRect MummyWarrior::getBounds() const


{
    EnemyFrame fr;
    if (crumbling)
    
{
    
    fr = crumbleFrames[animFrame % CRUMBLE_FRAMES];
    
}
    else
    
{
    
    fr = walkFrames[animFrame % WALK_FRAMES];
    
}
    return sf::FloatRect(x, y, (float)fr.w * SCALE, (float)fr.h * SCALE);
}

void MummyWarrior::takeDamage(int amount)


{
    if (crumbling || dying || DamageState == Dead)
    {
        return;
    }

    if (amount == 999 || amount == 20) 
    {
        Hp = 0;
        DamageState = Dead;
        dying = true;
        deathTimer = 0;
        deathFrame = 0;
        velocityX = 0;
        velocityY = 0;
    }
    else if (amount == 5) 
    {
        Hp -= amount;
        if (Hp <= 0)
        

{
            Hp = 0;
            DamageState = Dead;
            dying = true;
            deathTimer = 0;
            deathFrame = 0;
            velocityX = 0;
            velocityY = 0;
        }
    }
    else 
    {
        crumbling = true;
        crumbleTimer = 3.f;
        animFrame = 0;
        animTimer = 0;
        velocityX = 0;
    }
}

// =============================================================================
// =============================================================================

const float ZombieEnemy::WALK_FRAME_SPEED = 0.15f;
const float ZombieEnemy::SCALE = 3.f;

ZombieEnemy::ZombieEnemy(float x, float y) : Enemy(x, y, 8, 40.f, 0), stateTimer(0) 

{
    // texture.loadFromFile("Sprites/Enemies/Zombies.png");
    // sprite.setTexture(texture);
}
void ZombieEnemy::update(float dt, float playerX, float playerY) 

{
    updateDamageState(dt);
    if (dying) 

{

    velocityX = 0;
    velocityY = 0;
    deathTimer += dt;
    if (deathTimer >= 1.f) alive = false;
    return;

}
    velocityY += GRAVITY * dt;
    if (playerX < x) 

{

    velocityX = -speed;
    facingRight = false;

}
    else 

{

    velocityX = speed;
    facingRight = true;

}
    x += velocityX * dt;
    y += velocityY * dt;
    animTimer += dt; if (animTimer >= WALK_FRAME_SPEED) 

{

    animTimer = 0;
    animFrame = (animFrame + 1) % 4;

}
}

// Draws current frame buffer pixels to visual display
void ZombieEnemy::render(sf::RenderWindow& window, float camX, float camY) 

{
    if (!alive)
    {
        return;
    }
    sprite.setTextureRect(sf::IntRect(animFrame * 50, 0, 50, 60));
    float drawX = x - camX;
    float drawY = y - camY;
    if (facingRight) 

{

    sprite.setScale(-SCALE, SCALE);
    sprite.setPosition(drawX + 50.f * SCALE, drawY);

}
    else 

{

    sprite.setScale(SCALE, SCALE);
    sprite.setPosition(drawX, drawY);

}
    window.draw(sprite);
}
sf::FloatRect ZombieEnemy::getBounds() const 

{

    return sf::FloatRect(x, y, 50.f * SCALE, 60.f * SCALE);

}
const float FlyingTara::SCALE = 3.f;

const EnemyFrame FlyingTara::moveFrames[3] = {
    {343, 38, 80, 38},
    {426, 38, 80, 49},
    {510, 38, 80, 46}
};

FlyingTara::FlyingTara(float x, float y)
    : Enemy(x, y, 15, 120.f, 2.5f), AerialVehicle(x, y), shootCooldown(2.5f)


{
    if (!texture.loadFromFile("Sprites/Enemies/Flying Tara.png"))
    

{
    

    texture.loadFromFile("Sprites/Enemies/Enemy - Flying Tara.png");
    

}
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(moveFrames[0].x, moveFrames[0].y, moveFrames[0].w, moveFrames[0].h));
}

void FlyingTara::update(float dt, float playerX, float playerY)


{
    updateDamageState(dt);

    if (dying)
    

{
        velocityX = 0;
        velocityY = 0;
        deathTimer += dt;
        if (deathTimer >= 1.f)
        {
            Enemy::alive = false;
        }
        return;
    }

    if (playerX < Enemy::x)
    

{
    

    Enemy::facingRight = false;
    

}
    else
    

{
    

    Enemy::facingRight = true;
    

}
    if (Enemy::facingRight)
    
{
    
    velocityX = 120;
    
}
    else
    
{
    
    velocityX = -120;
    
}
    Enemy::x += velocityX * dt;

    if (Enemy::x < 100.f) 

{

    Enemy::x = 100.f;
    Enemy::facingRight = true;

}
    if (Enemy::x > 7900.f) 

{

    Enemy::x = 7900.f;
    Enemy::facingRight = false;

}
    // Cycle movement frames
    Enemy::animTimer += dt;
    if (Enemy::animTimer >= 0.15f)
    

{
        Enemy::animTimer = 0;
        Enemy::animFrame = (Enemy::animFrame + 1) % 3;
    }

    if (shootCooldown > 0)
    {
        shootCooldown -= dt;
    }
}


// Draws current frame buffer pixels to visual display

void FlyingTara::render(sf::RenderWindow& window, float camX, float camY)


{
    if (!Enemy::alive)
    {
        return;
    }

    EnemyFrame fr = moveFrames[Enemy::animFrame % 3];
    sprite.setTextureRect(sf::IntRect(fr.x, fr.y, fr.w, fr.h));

    sprite.setOrigin((float)fr.w / 2.f, (float)fr.h / 2.f);

    float drawX = Enemy::x - camX;
    float drawY = Enemy::y - camY;

    if (Enemy::facingRight)
    

{
    

    sprite.setScale(-SCALE, SCALE);
    

}
    else
    

{
    

    sprite.setScale(SCALE, SCALE);
    

}
    sprite.setPosition(drawX, drawY);
    window.draw(sprite);
}

sf::FloatRect FlyingTara::getBounds() const


{
    float w = 80.f * SCALE;
    float h = 38.f * SCALE;
    return sf::FloatRect(Enemy::x - w / 2.f, Enemy::y - h / 2.f, w, h);
}

Projectile* FlyingTara::shoot(float targetX, float targetY)


{
    if (shootCooldown <= 0 && std::abs(Enemy::x - targetX) < 250.f)
    

{
        shootCooldown = 2.5f;

        float vx;
        if (facingRight)
        
{
        
    vx = 120;
        
}
        else
        
{
        
    vx = -120;
        
}
        float vy = 50.f; // Small initial downward impulse

        return new GrenadeProjectile(Enemy::x, Enemy::y + 10.f, vx, vy, 1, false);
    }
    return NULL;
}

void FlyingTara::takeDamage(int amount)


{
    if (damageTimer > 0)
    {
        return;
    }

    int reducedAmount = amount / 3;
    if (reducedAmount < 1)
    {
        reducedAmount = 1;
    }

    Enemy::takeDamage(reducedAmount);
}

const float AlienEnemy::SCALE = 3.f;
AlienEnemy::AlienEnemy(float x, float y) : Enemy(x, y, 8, 50.f, 2.f), shootCooldown(1.5f) 

{
    texture.loadFromFile("Sprites/Enemies/Martian.png");
    sprite.setTexture(texture);
}
void AlienEnemy::update(float dt, float playerX, float playerY) 

{
    updateDamageState(dt);
    if (dying) 

{

    velocityX = 0;
    velocityY = 0;
    deathTimer += dt;
    if (deathTimer >= 1.f) alive = false;
    return;

}
    if (playerX < x) 

{

    velocityX = -speed;
    facingRight = false;

}
    else 

{

    velocityX = speed;
    facingRight = true;

}
    x += velocityX * dt;
    if (shootCooldown > 0)
    {
        shootCooldown -= dt;
    }
}

// Draws current frame buffer pixels to visual display
void AlienEnemy::render(sf::RenderWindow& window, float camX, float camY) 

{
}
sf::FloatRect AlienEnemy::getBounds() const 

{

    return sf::FloatRect(x, y, 45.f * SCALE, 55.f * SCALE);

}
Projectile* AlienEnemy::shoot(float targetX, float targetY) 

{
    if (shootCooldown <= 0 && std::abs(x - targetX) < 500.f) 

{
        shootCooldown = 2.f;
        float angle = angleToTarget(targetX, targetY);
        float pSpeed = 250.f;
        return new StraightProjectile(x + 22.f * SCALE, y + 27.f * SCALE, cos(angle) * pSpeed, sin(angle) * pSpeed, 1, false, sf::Color::Magenta, 10.f, 10.f);
    }
    return NULL;
}

const float AerialEnemy::SCALE = 3.f;
AerialEnemy::AerialEnemy(float x, float y) : Enemy(x, y, 6, 70.f, 2.f), shootCooldown(1.8f) 

{
    texture.loadFromFile("Sprites/Slug Flyer.png");
    sprite.setTexture(texture);
}
void AerialEnemy::update(float dt, float playerX, float playerY) 

{
    updateDamageState(dt);
    if (dying) 

{

    velocityX = 0;
    velocityY = 0;
    deathTimer += dt;
    if (deathTimer >= 1.f) alive = false;
    return;

}
    if (playerX < x) 

{

    velocityX = -speed;
    facingRight = false;

}
    else 

{

    velocityX = speed;
    facingRight = true;

}
    x += velocityX * dt;
    if (shootCooldown > 0)
    {
        shootCooldown -= dt;
    }
}

// Draws current frame buffer pixels to visual display
void AerialEnemy::render(sf::RenderWindow& window, float camX, float camY) 

{
}
sf::FloatRect AerialEnemy::getBounds() const 

{

    return sf::FloatRect(x, y, 75.f * SCALE, 45.f * SCALE);

}
Projectile* AerialEnemy::shoot(float targetX, float targetY) 

{
    if (shootCooldown <= 0 && std::abs(x - targetX) < 450.f) 

{
        shootCooldown = 2.2f;
        float angle = angleToTarget(targetX, targetY);
        float pSpeed = 300.f;
        return new StraightProjectile(x + 37.f * SCALE, y + 22.f * SCALE, cos(angle) * pSpeed, sin(angle) * pSpeed, 1, false, sf::Color::Yellow, 8.f, 8.f);
    }
    return NULL;
}

const float M15ABradley::SCALE = 3.f;

const EnemyFrame M15ABradley::moveFrames[6] = {
    {84, 382, 79, 51},
    {408, 382, 79, 51},
    {327, 381, 79, 52},
    {246, 382, 79, 51},
    {165, 381, 79, 52},
    {84, 382, 79, 51}
};

const EnemyFrame M15ABradley::loadFrames[6] = {
    {84, 382, 79, 51},
    {3, 565, 79, 51},
    {84, 559, 79, 57},
    {247, 551, 80, 65},
    {329, 547, 80, 69},
    {411, 544, 80, 72}
};

const EnemyFrame M15ABradley::deathFrames[6] = {
    {3, 971, 80, 78},
    {85, 971, 80, 78},
    {167, 969, 81, 80},
    {250, 1001, 81, 48},
    {402, 969, 31, 53},
    {213, 1069, 100, 66}
};

M15ABradley::M15ABradley(float x, float y)
    : Enemy(x, y - 48.f * SCALE, 21, 30.f, 4.0f), GroundVehicle(x, y - 48.f * SCALE), loadingRocket(false), loadTimer(0), loadFrame(0), shotFired(false)


{
    fireTimer = fireRate;
    if (!texture.loadFromFile("Sprites/Enemies/M15A.png"))
    

{
        if (!texture.loadFromFile("Sprites/Enemies/M-15.png"))
        

{
        

    texture.loadFromFile("Sprites/Enemies/Enemy - M-15A ''Bradley''.png");
        

}
    }
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(moveFrames[0].x, moveFrames[0].y, moveFrames[0].w, moveFrames[0].h));
    sprite.setScale(SCALE, SCALE);
}

void M15ABradley::update(float dt, float playerX, float playerY)


{
    if (Enemy::x > 2080.f && Enemy::x < 4000.f) 

{
        Enemy::x = 2080.f;
        velocityX = -velocityX; // bounce back
    }
    Enemy::update(dt, playerX, playerY);

    if (dying)
    

{
        velocityX = 0;
        deathTimer += dt;
        animTimer += dt;
        if (animTimer >= 0.15f)
        

{
            animTimer = 0;
            if (deathFrame < DEATH_FRAMES - 1)
            

{
            

    deathFrame++;
            

}
            else
            

{
            

    Enemy::alive = false;
            

}
        }
        return;
    }

    // fireTimer is managed (decremented) in Enemy::update()
    if (fireTimer <= 0 && !loadingRocket)
    

{
        loadingRocket = true;
        loadFrame = 0;
        loadTimer = 0;
        shotFired = false;
    }

    if (loadingRocket)
    

{
        velocityX = 0;
        loadTimer += dt;
        if (loadTimer >= 0.15f)
        

{
            loadTimer = 0;
            loadFrame++;
            if (loadFrame == 5 && !shotFired)
            

{
                float rx;
                if (facingRight)
                
{
                
    rx = Enemy::x + 80;
                
}
                else
                
{
                
    rx = Enemy::x - 20;
                
}
                float ry = Enemy::y - 50.f;
                float dx = playerX - rx;
                float dy = playerY - ry;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < 1.f)
                {
                    dist = 1.f;
                }
                float rSpeed = 350.f;
                float vx = (dx / dist) * rSpeed;
                float vy = (dy / dist) * rSpeed - 150.f;
                if (GameData::activeEnemyManager)
                

{
                

    GameData::activeEnemyManager->addProjectile(new M15ARocketProjectile(rx, ry, vx, vy));
                

}
                shotFired = true;
            }
            if (loadFrame >= LOAD_FRAMES)
            

{
                loadingRocket = false;
                fireTimer = fireRate;
                shotFired = false;
                loadFrame = 0;
            }
        }
    }
    else
    

{
        facingRight = (playerX > Enemy::x);
        if (facingRight)
        
{
        
    velocityX = Enemy::speed;
        
}
        else
        
{
        
    velocityX = -Enemy::speed;
        
}
        Enemy::x += velocityX * dt;

        animTimer += dt;
        if (animTimer >= 0.15f)
        

{
            animTimer = 0;
            animFrame = (animFrame + 1) % MOVE_FRAMES;
        }
    }
}


// Draws current frame buffer pixels to visual display

void M15ABradley::render(sf::RenderWindow& window, float camX, float camY)


{
    if (!Enemy::alive)
    {
        return;
    }

    EnemyFrame fr;
    if (dying)
    

{
    

    fr = deathFrames[deathFrame % DEATH_FRAMES];
    

}
    else if (loadingRocket)
    

{
    

    fr = loadFrames[loadFrame % LOAD_FRAMES];
    

}
    else
    

{
    

    fr = moveFrames[animFrame % MOVE_FRAMES];
    

}
    float h = 48.f * SCALE; // stable height of the tank walking frame

    sprite.setTextureRect(sf::IntRect(fr.x, fr.y, fr.w, fr.h));
    sprite.setOrigin((float)fr.w / 2.f, (float)fr.h);
    sprite.setPosition(Enemy::x - camX, Enemy::y - camY + h + 15.f);

    if (facingRight)
        sprite.setScale(-SCALE, SCALE);
    else
        sprite.setScale(SCALE, SCALE);

    window.draw(sprite);
}

sf::FloatRect M15ABradley::getBounds() const


{
    float w = 79.f * SCALE;
    float h = 48.f * SCALE;
    return sf::FloatRect(Enemy::x - w / 2.f, Enemy::y, w, h);
}

void M15ABradley::takeDamage(int amount)


{
    if (damageTimer > 0)
    {
        return;
    }

    int reducedAmount = amount / 3;
    if (reducedAmount < 1)
    {
        reducedAmount = 1;
    }

    Enemy::takeDamage(reducedAmount);
}

const float EnemySub::SCALE = 3.5f;

const EnemyFrame EnemySub::moveFrames[4] = {
    {222, 19, 65, 34},
    {362, 19, 67, 34},
    {81, 19, 65, 34},
    {433, 21, 66, 32}
};

EnemySub::EnemySub(float x, float y) : Enemy(x, y, 15, 60.f, 2.5f), shootCooldown(2.5f) 

{
    if (!texture.loadFromFile("Sprites/Enemies/Enemy Submarine.png"))
    

{
    

    texture.loadFromFile("Sprite/Enemies/Enemy Submarine.png");
    

}
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(moveFrames[0].x, moveFrames[0].y, moveFrames[0].w, moveFrames[0].h));
}

void EnemySub::update(float dt, float playerX, float playerY) 

{
    // Submerged update
    // Clamp to underwater biome 
    if (Enemy::x < 2100.f) 

{

    Enemy::x = 2100.f;
    velocityX = -velocityX;

}
    if (Enemy::x > 4000.f) 

{

    Enemy::x = 4000.f;
    velocityX = -velocityX;

}
    if (Enemy::y < 580.f) 

    { Enemy::y = 580.f;
    velocityY = std::abs(velocityY);
    }; // Ensure it bounces downwards

    updateDamageState(dt);
    if (dying) 

{

    velocityX = 0;
    velocityY = 0;
    deathTimer += dt;
    if (deathTimer >= 1.f) alive = false;
    return;

}
    facingRight = (playerX > x);

    if (facingRight)
    
{
    
    velocityX = speed;
    
}
    else
    
{
    
    velocityX = -speed;
    
}
    Enemy::x += velocityX * dt; // Submarine moves slowly towards the player!

    if (shootCooldown > 0)
    {
        shootCooldown -= dt;
    }

    animTimer += dt;
    if (animTimer >= 0.15f)
    

{
        animTimer = 0;
        animFrame = (animFrame + 1) % 4;
    }
}


// Draws current frame buffer pixels to visual display

void EnemySub::render(sf::RenderWindow& window, float camX, float camY) 

{
    if (!alive)
    {
        return;
    }

    EnemyFrame fr = moveFrames[animFrame % 4];
    sprite.setTextureRect(sf::IntRect(fr.x, fr.y, fr.w, fr.h));

    if (dying) 

{

    sprite.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>((1.0f - deathTimer) * 255)));

}
    else 

{

    sprite.setColor(sf::Color::White); // Color: White

}
    float drawX = x - camX;
    float drawY = y - camY;

    if (facingRight) 

{
        sprite.setScale(-SCALE, SCALE);
        sprite.setPosition(drawX + fr.w * SCALE, drawY);
    }
    else 

{
        sprite.setScale(SCALE, SCALE);
        sprite.setPosition(drawX, drawY);
    }
    window.draw(sprite);
}

sf::FloatRect EnemySub::getBounds() const 

{
    float w = 65.f * SCALE;
    float h = 34.f * SCALE;
    return sf::FloatRect(x, y, w, h);
}

Projectile* EnemySub::shoot(float targetX, float targetY) 

{
    if (shootCooldown <= 0 && std::abs(x - targetX) < 550.f) 

{
        shootCooldown = 2.5f;
        float rx;
        if (facingRight)
        
{
        
    rx = x + 65 * SCALE;
        
}
        else
        
{
        
    rx = x;
        
}
        float ry = y - 10.f;
        float dx = targetX - rx;
        float dy = targetY - ry;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < 1.f)
        {
            dist = 1.f;
        }
        float rSpeed = 350.f;
        float vx = (dx / dist) * rSpeed;
        float vy = (dy / dist) * rSpeed - 150.f; // Launch rockets upwards/towards player
        return new M15ARocketProjectile(rx, ry, vx, vy);
    }
    return NULL;
}

const float POWPrisoner::SCALE = 3.f;
POWPrisoner::POWPrisoner(float x, float y) : Enemy(x, y, 1, 0, 0), rescued(false) 

{
    // texture.loadFromFile("Sprites/NPCS and Prisoners.png");
    // sprite.setTexture(texture);
}
void POWPrisoner::update(float dt, float playerX, float playerY) 

{
    updateDamageState(dt);
    if (dying) 

{

    velocityX = 0;
    velocityY = 0;
    deathTimer += dt;
    if (deathTimer >= 1.f) alive = false;
    return;

}
}

// Draws current frame buffer pixels to visual display
void POWPrisoner::render(sf::RenderWindow& window, float camX, float camY) 

{
    if (!alive)
    {
        return;
    }
    sprite.setTextureRect(sf::IntRect(0, 0, 35, 55));
    float drawX = x - camX;
    float drawY = y - camY;
    sprite.setScale(SCALE, SCALE);
    sprite.setPosition(drawX, drawY);
    window.draw(sprite);
}
sf::FloatRect POWPrisoner::getBounds() const 

{

    return sf::FloatRect(x, y, 35.f * SCALE, 55.f * SCALE);

}
void EnemyManager::updateLevel2(float dt, Soldier* player, const World& world)


{


    updateDynamicLevel(dt, player, world, g_Level2Spawns, G_LEVEL2_SPAWN_COUNT);


}
void EnemyManager::updateLevel3(float dt, Soldier* player, const World& world)


{


    updateDynamicLevel(dt, player, world, g_Level3Spawns, G_LEVEL3_SPAWN_COUNT);


}
void EnemyManager::updateDynamicLevel(float dt, Soldier* player, const World& world, LevelSpawn* spawns, int spawnCount)


{
    GameData::activeEnemyManager = this;
    if (!player)
    {
        return;
    }
    float playerX = player->getX();
    float playerY = player->getY();

    for (int i = 0; i < spawnCount; i++)
    

{
        if (!spawns[i].spawned && std::abs(playerX - spawns[i].x) < 800.f)
        

{
            spawns[i].spawned = true;
            float sx = spawns[i].x;
            int tileCol = (int)(sx / CELL);
            float sy = findGroundY(tileCol, playerY, world);

            switch (spawns[i].type)
            

{
            case 0:
                for (int j = 0; j < 3; j++)
                    spawnEnemy(new RebelSoldier(sx + j * 40.f, findGroundY((int)((sx + j * 40.f) / CELL), playerY, world)));
                break;
            case 1:
                for (int j = 0; j < 2; j++)
                    spawnEnemy(new ShieldedSoldier(sx + j * 50.f, findGroundY((int)((sx + j * 50.f) / CELL), playerY, world)));
                break;
            case 2:
                for (int j = 0; j < 2; j++)
                    spawnEnemy(new BazookaSoldier(sx + j * 50.f, findGroundY((int)((sx + j * 50.f) / CELL), playerY, world)));
                break;
            case 3:
                for (int j = 0; j < 2; j++)
                    spawnEnemy(new GrenadeThrower(sx + j * 50.f, findGroundY((int)((sx + j * 50.f) / CELL), playerY, world)));
                break;
            case 4:
                for (int j = 0; j < 3; j++)
                    spawnEnemy(new MummyWarrior(sx + j * 40.f, findGroundY((int)((sx + j * 40.f) / CELL), playerY, world)));
                break;
            case 5:
                for (int j = 0; j < 3; j++)
                    spawnEnemy(new ZombieEnemy(sx + j * 40.f, findGroundY((int)((sx + j * 40.f) / CELL), playerY, world)));
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                for (int j = 0; j < 2; j++)
                    spawnEnemy(new FlyingTara(sx + j * 80.f, sy - 435.f)); 
                break;
            case 9:
                spawnEnemy(new M15ABradley(sx, sy));
                break;
            case 10:
                spawnEnemy(new POWPrisoner(sx, sy));
                break;
            case 11:
                if (playerX >= 2080.f && playerY >= 500.f && world.getBiomeType(playerX) == 1)
                

{
                    float spawnX = playerX + 400.f; 
                    float spawnY = 580.f; 
                    spawnEnemy(new EnemySub(spawnX, spawnY));
                }
                else
                

{
                

    spawns[i].spawned = false; // keep trying until player enters water in aquatic biome
                

}
                break;
            case 12:
                for (int j = 0; j < 3; j++)
                    spawnEnemy(new Paratrooper(sx + j * 50.f, 100.f)); 
                break;
            }
        }
    }

    for (int i = 0; i < count; i++)
    

{
        if (!enemies[i] || !enemies[i]->isAlive() )
        {
            continue;
        }

        if (enemies[i]->getType() == Enemy::TYPE_MUMMY)
        {
            MummyWarrior* mummy = static_cast<MummyWarrior*>(enemies[i]);
            if (!mummy->isCrumbling() && mummy->getBounds().intersects(player->getBounds()))
            {
                if (player->getTransformState() != 2)
                {
                    player->setTransformationState(new MummyState());
                }
            }
        }

        if (enemies[i]->getType() == 12)
        

{
            POWPrisoner* pow = static_cast<POWPrisoner*>(enemies[i]);
            if (!pow->isRescued() && pow->getBounds().intersects(player->getBounds()))
            

{
                pow->rescue(); // Prevents this block from executing multiple times before object clean-up!
                int r = rand() % 3;
                if (r == 0)
                {
                    player->pickupWeapon(new HeavyMachineGun());
                }
                else if (r == 1) player->pickupWeapon(new RocketLauncher());
                else player->pickupWeapon(new FlameShot());
                enemies[i]->takeDamage(999);
            }
        }

        enemies[i]->update(dt, playerX, playerY);
        if (enemies[i]->getType() != 7 && enemies[i]->getType() != 11) // Tara = 7, Sub = 11 (based on implementation numbers)
        

{
            resolveEnemyV(*enemies[i], world);
            resolveEnemyH(*enemies[i], world);
        }

        
        if (enemies[i]->getType() == Enemy::TYPE_PARATROOPER && enemies[i]->isOnGround())
        

{
            float spX = enemies[i]->getX();
            float spY = enemies[i]->getY();

            delete enemies[i];
            enemies[i] = NULL;

            int r = rand() % 4;
            if (r == 0)
            {
                enemies[i] = new RebelSoldier(spX, spY);
            }
            else if (r == 1) enemies[i] = new ShieldedSoldier(spX, spY);
            else if (r == 2) enemies[i] = new BazookaSoldier(spX, spY);
            else enemies[i] = new GrenadeThrower(spX, spY);
        }

        if (!enemies[i])
        {
            continue;
        }

        Projectile* p = enemies[i]->shoot(playerX, playerY);
        if (p)
        

{
        

    addProjectile(p);
        

}
    }

    for (int i = 0; i < projCount; i++)
        if (projectiles[i] && projectiles[i]->isAlive())
            projectiles[i]->update(dt, world, playerX, playerY);

    for (int pIdx = 0; pIdx < projCount; pIdx++)
    

{
        if (projectiles[pIdx] && projectiles[pIdx]->isAlive() && !projectiles[pIdx]->isFromPlayer() && !projectiles[pIdx]->getHasDamagedPlayer())
        

{
            if (player && player->isAlive() && player->getBounds().intersects(projectiles[pIdx]->getBounds()))
            

{
                int dmg = projectiles[pIdx]->getDamage();
                if (dynamic_cast<BlastProjectile*>(projectiles[pIdx]) != NULL)
                {
                    dmg = 6;
                }
                else if (dmg > 3) dmg = 3;

                player->takeDamage(dmg);
                projectiles[pIdx]->setHasDamagedPlayer(true);
                projectiles[pIdx]->onHit();
            }
        }
    }

    for (int pIdx = 0; pIdx < projCount; pIdx++)
    

{
        if (projectiles[pIdx] && projectiles[pIdx]->isAlive() && projectiles[pIdx]->isFromPlayer())
        

{
            sf::FloatRect projBounds = projectiles[pIdx]->getBounds();
            for (int eIdx = 0; eIdx < count; eIdx++)
            

{
                if (enemies[eIdx] && enemies[eIdx]->isAlive())
                

{
                    if (enemies[eIdx]->getBounds().intersects(projBounds))
                    

{
                        bool blocked = false;
                        if (enemies[eIdx]->getType() == Enemy::TYPE_SHIELDED)
                        

{
                            ShieldedSoldier* s = (ShieldedSoldier*)enemies[eIdx];
                            if (s->isHitBlocked(projectiles[pIdx]->getX()))
                                blocked = true;
                        }

                        if (blocked)
                        

{
                            projectiles[pIdx]->onHit();
                            break;
                        }

                        enemies[eIdx]->takeDamage(projectiles[pIdx]->getDamage());
                        projectiles[pIdx]->onHit();

                        if (!projectiles[pIdx]->isFlamePool())
                        

{
                        

    break;
                        

}
                    }
                }
            }
        }
    }

    removeDeadEnemies();
    removeDeadProjectiles();
}








#pragma once
#include "entities.h"
#include "Weapon.h"
#include <SFML/Graphics.hpp>

// Projectile

class Projectile;

// Grenade

class Grenade;

// Knife

class Knife;

// Transformation State

class TransformationState
{
protected:
    int type;
    float timer;
public:
    static const int normal;
    static const int undead;
    static const int mummy;

    TransformationState() : type(normal), timer(0) 

{

}
    virtual ~TransformationState() 

{

}
    virtual void enter(class Soldier& soldier) = 0;
    virtual void update(class Soldier& soldier, float dt) = 0;
    virtual void exit(class Soldier& soldier) = 0;
    int getType() const 

{

    return type;

}
    bool isExpired() const 

{

    return timer <= 0 && type != normal;

}
};

// Normal State

class NormalState : public TransformationState
{
public:
    NormalState() 

{

    type = normal;

}
    void enter(class Soldier& soldier) 
{ 
}
    void update(class Soldier& soldier, float dt) 
{
}
    void exit(class Soldier& soldier) 
{ 
}
};

// Undead State

class UndeadState : public TransformationState
{
public:
    UndeadState() 

{

    type = undead;
    timer = 10.f;

}
    void enter(class Soldier& soldier);
    void update(class Soldier& soldier, float dt);
    void exit(class Soldier& soldier);
};

// Mummy State

class MummyState : public TransformationState
{
public:
    MummyState() 

{

    type = mummy;
    timer = 10.f;

}
    void enter(class Soldier& soldier);
    void update(class Soldier& soldier, float dt);
    void exit(class Soldier& soldier);
};

struct SpriteFrame {
    int x, y, w, h;
};

// Soldier

class Soldier : public DamagableEntity
{
protected:
    const char* name;
    int lives;
    int grenades;
    bool facingRight;
    bool onGround;
    bool wasOnGround;
    bool useFireGrenade;

    float velocityX, velocityY;
    float jumpForce;
    static const float GRAVITY;

    Weapon* pistol;
    Weapon* currentWeapon;
    Knife* knife;

    TransformationState* transformState;

    float specialTimer;
    bool specialActive;

    sf::Texture texture;
    sf::Sprite sprite;
    sf::Texture legTexture;
    sf::Sprite legSprite;

    float animTimer;
    int animFrame;
    float hitWindowTimer;
    float flameHoldTimer;
    bool flameInterrupted;

    int legAnimFrame;
    float legAnimTimer;
    int knifeAnimFrame;
    float knifeAnimTimer;
    bool knifeAttacking;
    bool sWasDown;
    bool lWasDown;
    bool gWasDown;
    bool hWasDown;

    // weapon animation
    float weaponAnimTimer;
    int weaponAnimFrame;
    int weaponAnimState;

    sf::Sprite fireSprite;

public:
    bool renderOnlyUpper;
    int currentWeaponAnim;
    int weaponIndex;

    static const int WEAPON_PISTOL;
    static const int WEAPON_HMG;
    static const int WEAPON_ROCKET;
    static const int WEAPON_FLAME;
    static const int WEAPON_LASER;
    static const int WEAPON_KNIFE;

    static const int BASE_GRENADES;
    static const float BASE_PISTOL_DMG;
    static const float BASE_KNIFE_DMG;
    static const float BASE_FIRE_RATE;
    static const float BASE_WALK_SPEED;
    static const float MELEE_COOLDOWN;
    static const int BASE_HP;
    static const int BASE_LIVES;

    static const int WEAPON_ANIM_IDLE;
    static const int WEAPON_ANIM_FIRE;
    static const int WEAPON_ANIM_RELOAD;

    Soldier(float x, float y, const char* name);
    virtual ~Soldier();


    // Draws current frame buffer pixels to visual display

    void render(sf::RenderWindow& window);

    // Advances state calculations by frame time increment
    void update(float dt);
    virtual void render(sf::RenderWindow& window, float camX, float camY);
    sf::FloatRect getBounds() const;

    virtual void handleInput(float dt);
    virtual Projectile** shoot(float angleRad, int& outCount);
    virtual Grenade* throwGrenade(float angleRad);
    virtual bool tryMelee(const sf::FloatRect& targetBounds);

    void setTransformationState(TransformationState* state);
    int getTransformState() const;

    virtual void pickupWeapon(Weapon* weapon);
    virtual void cycleWeapon();
    Weapon* getCurrentWeapon() const 

{

    return currentWeapon;

}
    virtual float getFireRateMultiplier() const 

{

    return 1.f;

}
    virtual float getSpeedMultiplier() const 

{

    return 1.f;

}
    virtual float getMeleeDamageMultiplier() const 

{

    return 1.f;

}
    virtual int getStartingGrenades() const 

{

    return BASE_GRENADES;

}
    virtual bool canMelee() const 

{

    return true;

}
    virtual void activateSpecialPower() = 0;
    virtual int getRunFrameCount() const 

{

    return 12;

}
    void takeDamage(int amount);

    int getLives() const 

{

    return lives;

}
    float getHitWindowTimer() const 

{

    return hitWindowTimer;

}
    int getGrenades() const 

{

    return grenades;

}
    bool isFacingRight() const 

{

    return facingRight;

}
    void setFacingRight(bool b) 

{

    facingRight = b;

}
    bool isOnGround() const 

{

    return onGround;

}
    void setOnGround(bool b) 

{

    onGround = b;

}
    const char* getName() const 

{

    return name;

}
    float getVelX() const 

{

    return velocityX;

}
    float getVelY() const 

{

    return velocityY;

}
    void setVelX(float v) 

{

    velocityX = v;

}
    void setVelY(float v) 

{

    velocityY = v;

}
    void onDeath();
    void reset();
};

// Player Soldier

class PlayerSoldier : public Soldier
{
public:
    PlayerSoldier(float x, float y, const char* name);


    void handleInput(float dt);
    virtual void activateSpecialPower() = 0;
    float getAimAngle() const 

{

    return aimAngle;

}
protected:
    float aimAngle;
};

// Marco

class Marco : public PlayerSoldier
{
public:
    Marco(float x, float y);

    void render(sf::RenderWindow& window);
    void render(sf::RenderWindow& window, float camX, float camY);
    float getFireRateMultiplier() const 

{

    return 1.25f;

}
    int getStartingGrenades() const 

{

    return BASE_GRENADES - 2;

}
    void activateSpecialPower();

    // Advances state calculations by frame time increment
    void update(float dt);
    Projectile** shoot(float angleRad, int& outCount);
    void cycleWeapon();
    int getRunFrameCount() const;

private:
    bool dualFireActive;
    float dualFireTimer;

    sf::Texture marcoSheet;

    static const int PISTOL_WALK_FRAMES = 5;
    static const SpriteFrame pistolWalkFrames[5];

    static const int PISTOL_FIRE_FRAMES = 6;
    static const SpriteFrame pistolFireFrames[6];

    static const int HMG_WALK_FRAMES = 5;
    static const SpriteFrame hmgWalkFrames[5];

    static const int HMG_FIRE_FRAMES = 3;
    static const SpriteFrame hmgFireFrames[3];

    static const int KNIFE_IDLE_FRAMES = 1;
    static const SpriteFrame knifeIdleFrames[1];

    static const int KNIFE_ATTACK_FRAMES = 4;
    static const SpriteFrame knifeAttackFrames[4];

    static const int LEG_FRAMES = 4;
    static const SpriteFrame legFrames[4];

};

// Eri

class Eri : public PlayerSoldier
{
public:
    Eri(float x, float y);

 
    void render(sf::RenderWindow& window);

  
    void render(sf::RenderWindow& window, float camX, float camY);
    float getFireRateMultiplier() const 

{

    return 0.8f;

}
    int getStartingGrenades() const 

{

    return BASE_GRENADES * 2;

}
    bool canMelee() const 

{

    return false;

}
    void activateSpecialPower();

    // Advances state calculations by frame time increment
    void update(float dt);
    Grenade* throwGrenade(float angle);
    void cycleWeapon();
    int getRunFrameCount() const;

private:
    bool doubleGrenadeActive;
    float doubleGrenadeTimer;

    sf::Texture eriSheet;

    static const int PISTOL_WALK_FRAMES = 12;
    static const SpriteFrame pistolWalkFrames[12];

    static const int LEG_FRAMES = 15;
    static const SpriteFrame legFrames[15];

    static const int PISTOL_FIRE_FRAMES = 9;
    static const SpriteFrame pistolFireFrames[9];

    static const int HMG_WALK_FRAMES = 7;
    static const SpriteFrame hmgWalkFrames[7];

    static const int HMG_FIRE_FRAMES = 12;
    static const SpriteFrame hmgFireFrames[12];

    static const int KNIFE_FRAMES = 9;
    static const SpriteFrame knifeFrames[9];

};

// Fio

class Fio : public PlayerSoldier
{
public:
    Fio(float x, float y);

    void render(sf::RenderWindow& window);

    void render(sf::RenderWindow& window, float camX, float camY);
    float getFireRateMultiplier() const 

{

    return 1.f;

}
    int getStartingGrenades() const 

{

    return BASE_GRENADES + 4;

}
    void activateSpecialPower();

    void update(float dt);
    void cycleWeapon();
    int getRunFrameCount() const;

private:
    bool extraAmmoActive;
    float extraAmmoTimer;

    sf::Texture fioSheet;

    static const int PISTOL_WALK_FRAMES = 4;
    static const SpriteFrame pistolWalkFrames[4];

    static const int LEG_FRAMES = 4;
    static const SpriteFrame legFrames[4];

    static const int PISTOL_FIRE_FRAMES = 6;
    static const SpriteFrame pistolFireFrames[6];

    static const int HMG_WALK_FRAMES = 5;
    static const SpriteFrame hmgWalkFrames[5];

    static const int HMG_FIRE_FRAMES = 4;
    static const SpriteFrame hmgFireFrames[4];

    static const int KNIFE_WALK_FRAMES = 3;
    static const SpriteFrame knifeWalkFrames[3];

    static const int KNIFE_ATTACK_FRAMES = 4;
    static const SpriteFrame knifeAttackFrames[4];

};

// Tarma

class Tarma : public PlayerSoldier
{
public:
    Tarma(float x, float y);

    void render(sf::RenderWindow& window);

    void render(sf::RenderWindow& window, float camX, float camY);
    float getFireRateMultiplier() const 

{

    return 1.1f;

}
    int getStartingGrenades() const 

{

    return BASE_GRENADES;

}
    void activateSpecialPower();

    void update(float dt);
    void cycleWeapon();
    int getRunFrameCount() const;

private:
    bool armorActive;
    float armorTimer;

    sf::Texture tarmaSheet;

    static const int PISTOL_WALK_FRAMES = 14;
    static const SpriteFrame pistolWalkFrames[14];

    static const int LEG_FRAMES = 7;
    static const SpriteFrame legFrames[7];

    static const int PISTOL_FIRE_FRAMES = 7;
    static const SpriteFrame pistolFireFrames[7];

    static const int HMG_WALK_FRAMES = 5;
    static const SpriteFrame hmgWalkFrames[5];

    static const int HMG_FIRE_FRAMES = 3;
    static const SpriteFrame hmgFireFrames[3];

    static const int KNIFE_WALK_FRAMES = 2;
    static const SpriteFrame knifeWalkFrames[2];

    static const int KNIFE_ATTACK_FRAMES = 4;
    static const SpriteFrame knifeAttackFrames[4];

};








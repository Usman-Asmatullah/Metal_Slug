#pragma once
#include "entities.h"
#include <SFML/Graphics.hpp>

// Weapon

class Weapon;

// Projectile

class Projectile;

// Soldier

class Soldier;

// Vehicle

class Vehicle : public Entity {
protected:
    float speed;
    int maxHP;
    Soldier* pilot;
    Weapon* mainWeapon;

    float vehVelX, vehVelY;
    bool vehOnGround;
    float fireCooldownTimer;
public:
    Vehicle(float x = 0, float y = 0)
        : Entity(x, y), speed(0), maxHP(0), pilot(NULL), mainWeapon(NULL), vehVelX(0), vehVelY(0), vehOnGround(true), fireCooldownTimer(0) 

{

}
    virtual ~Vehicle() 

{

}
    virtual void move(float dx, float dy) = 0;

    // Executes weapon discharge spawning new projectile
    virtual void fire(Projectile*&) = 0;

    // Cleanups persistent resources upon entity removal
    virtual void onDestroyed() = 0;
    virtual void handleInput(float dt) = 0;
    void board(Soldier* s);
    void eject();

    virtual void render(sf::RenderWindow& window, float camX, float camY) = 0;

    bool isPiloted() const 

{

    return pilot != NULL;

}
    void setOnGround(bool g) 

{

    vehOnGround = g;

}
    bool isOnGround() const 

{

    return vehOnGround;

}
    void setVelocityY(float vy) 

{

    vehVelY = vy;

}
    float getVelocityY() const 

{

    return vehVelY;

}
    void setVelocityX(float vx) 

{

    vehVelX = vx;

}
    float getVelocityX() const 

{

    return vehVelX;

}
};

// Ground Vehicle

class GroundVehicle : public Vehicle
{
public:
    GroundVehicle(float x = 0, float y = 0) : Vehicle(x, y) { }

    virtual void update(float dt) = 0;
    virtual void handleInput(float dt) = 0;
    virtual void render(sf::RenderWindow& w) { render(w, 0, 0); }
    virtual void render(sf::RenderWindow& w, float camX, float camY) = 0;
    virtual sf::FloatRect getBounds() const { return sf::FloatRect(x, y, 0, 0); }
    virtual void move(float dx, float dy) { x += dx;
    y += dy;
    };

    virtual void fire(Projectile*& p) = 0;

    virtual void onDestroyed() = 0;
};

// Aerial Vehicle

class AerialVehicle : public Vehicle
{
public:
    AerialVehicle(float x = 0, float y = 0) : Vehicle(x, y) { }

    virtual void update(float dt) = 0;
    virtual void handleInput(float dt) = 0;
    virtual void render(sf::RenderWindow& w) { render(w, 0, 0); }
    virtual void render(sf::RenderWindow& w, float camX, float camY) = 0;
    virtual sf::FloatRect getBounds() const { return sf::FloatRect(x, y, 0, 0); }
    virtual void move(float dx, float dy) { x += dx;
    y += dy;
    };

    virtual void fire(Projectile*& p) = 0;

 
    virtual void onDestroyed() = 0;
};

// Aquatic Vehicle

class AquaticVehicle : public Vehicle
{
public:
    AquaticVehicle(float x = 0, float y = 0) : Vehicle(x, y) { }

    virtual void update(float dt) = 0;
    virtual void handleInput(float dt) = 0;
    virtual void render(sf::RenderWindow& w) { render(w, 0, 0); }
    virtual void render(sf::RenderWindow& w, float camX, float camY) = 0;
    virtual sf::FloatRect getBounds() const { return sf::FloatRect(x, y, 0, 0); }
    virtual void move(float dx, float dy) { x += dx;
    y += dy;
    };

    virtual void fire(Projectile*& p) = 0;


    virtual void onDestroyed() = 0;
};

// Metal Slug Tank

class MetalSlugTank : public GroundVehicle {
public:
    typedef int State;
    static const int IDLE = 0;
    static const int DRIVING = 1;
    static const int CROUCH_DRIVING = 2;
    static const int JUMPING = 3;
    static const int FIRING = 4;
    static const int ENTERING = 5;
    static const int EXITING = 6;
    static const float SCALE;

    MetalSlugTank(float startX, float startY);

    void update(float dt);

    void handleInput(float dt);

    void render(sf::RenderWindow& w, float camX, float camY);
    sf::FloatRect getBounds() const;
    void fire(Projectile*& p) { }
    void onDestroyed() { }

    void setState(State s);
    void setFacingRight(bool right) 

{

    facingRight = right;

}
private:
    sf::Texture tIdle, tDriving, tCDriving, tJump, tFire, tEnter, tExit;
    sf::Sprite sprite;
    State currentState;
    float animTimer;
    int currentFrame;
    bool facingRight;

    void updateFrameSize();
};

// Slug Flyer

class SlugFlyer : public AerialVehicle {
public:
    typedef int State;
    static const int IDLE_DRIVING = 0;
    static const int ENTER_TAKEOFF = 1;
    static const int LAND_EXIT = 2;
    static const float SCALE;

    SlugFlyer(float startX, float startY);

    void update(float dt);

    void handleInput(float dt);

    void render(sf::RenderWindow& w, float camX, float camY);
    sf::FloatRect getBounds() const;
    void fire(Projectile*& p) { }
    void onDestroyed() { }

    void setState(State s);
    void setFacingRight(bool right) 

{

    facingRight = right;

}
private:
    sf::Texture tIdleDriving, tEnter, tExit;
    sf::Sprite sprite;
    State currentState;
    float animTimer;
    int currentFrame;
    bool facingRight;

    void updateFrameSize();
};

// Slug Mariner

class SlugMariner : public AquaticVehicle {
public:
    typedef int State;
    static const int DRIVING = 0;
    static const int ENTERING = 1;
    static const int EXITING = 2;
    static const float SCALE;

    SlugMariner(float startX, float startY);

    void update(float dt);

    void handleInput(float dt);

    void render(sf::RenderWindow& w, float camX, float camY);
    sf::FloatRect getBounds() const;
    void fire(Projectile*& p) { }
    void onDestroyed() { }

    void setState(State s);
    void setFacingRight(bool right) 

{

    facingRight = right;

}
private:
    sf::Texture tDriving, tEnter, tExit;
    sf::Sprite sprite;
    State currentState;
    float animTimer;
    int currentFrame;
    bool facingRight;

    void updateFrameSize();
};









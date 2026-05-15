#pragma once
#include <SFML/Graphics.hpp>

// Projectile

class Projectile
{
protected:
    float x, y;
    float velx, vely;
    int damage;
    bool fromPlayer;
    bool alive;
    bool hasDamagedPlayer;

public:
    Projectile(float x, float y, float vx, float vy, int damage, bool fromPlayer) : x(x), y(y), velx(vx), vely(vy), damage(damage), fromPlayer(fromPlayer), alive(true), hasDamagedPlayer(false) 

{

}
    virtual ~Projectile() 

{

}
    virtual void update(float dt) = 0;
    virtual void update(float dt, const class World& world) 

{

    update(dt);

}
    virtual void update(float dt, const class World& world, float playerX, float playerY) 

{

    update(dt, world);

}
    virtual void render(sf::RenderWindow& window);
    virtual void render(sf::RenderWindow& window, float camX, float camY);
    virtual sf::FloatRect getBounds() const;

    virtual void onHit() 

{

    alive = false;

}
    int getDamage() const 

{

    return damage;

}
    bool isAlive() const 

{

    return alive;

}
    bool isFromPlayer() const 

{

    return fromPlayer;

}
    float getX() const 

{

    return x;

}
    float getY() const 

{

    return y;

}
    bool getHasDamagedPlayer() const 

{

    return hasDamagedPlayer;

}
    void setHasDamagedPlayer(bool val) 

{

    hasDamagedPlayer = val;

}
    virtual bool isExplosive() const 

{

    return false;

}
    virtual bool isFlame() const 

{

    return false;

}
    virtual bool isFlamePool() const 

{

    return false;

}
    virtual float getBlastRadius()const 

{

    return 0;

}
};

// Straight Projectile

class StraightProjectile : public Projectile
{
protected:
    sf::RectangleShape shape;

public:
    StraightProjectile(float x, float y, float vx, float vy,
        int damage, bool fromPlayer,
        sf::Color color = sf::Color::Yellow,
        float width = 8.0, float height = 4.0);


    // Advances state calculations by frame time increment

    void update(float dt);
    void update(float dt, const class World& world);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window, float camX, float camY);
    sf::FloatRect getBounds() const;


};

// Ballistic Projectile

class BallisticProjectile : public Projectile
{

private:
    float blastRadius;
    bool explosive;
    bool isFire;
protected:
    sf::CircleShape shape;
public:
    static const float GRAVITY;

    BallisticProjectile(float x, float y, float vx, float vy, int damage, bool fromPlayer, float blastRadius = 0, bool explosive = false, sf::Color color = sf::Color(150, 150, 150), bool isFire = false);


    // Advances state calculations by frame time increment

    void update(float dt);
    void update(float dt, const class World& world);
    void update(float dt, const class World& world, float playerX, float playerY);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window, float camX, float camY);
    sf::FloatRect getBounds() const;
    void onHit();

    bool isExplosive() const 

{

    return explosive;

}
    float getBlastRadius() const 

{

    return blastRadius;

}
};

// Flame Projectile

class FlameProjectile : public StraightProjectile
{
public:
    FlameProjectile(float x, float y, float vx, float vy, bool fromPlayer);


    
    void update(float dt);


    void render(sf::RenderWindow& window);

 
    void render(sf::RenderWindow& window, float camX, float camY);

    bool isFlame() const 

{

    return true;

}
    float getDPS() const 

{

    return 2.0f;

}
};

// Laser Projectile

class LaserProjectile : public Projectile
{
private:
    float length;
    bool facingRight;
    float lifetime = 0.1f;

protected:
    sf::RectangleShape beam;


public:
    LaserProjectile(float originX, float originY,
        float length, bool facingRight, bool fromPlayer);


  

    void update(float dt);


    void render(sf::RenderWindow& window);

   
    void render(sf::RenderWindow& window, float camX, float camY);
    sf::FloatRect getBounds() const;


};

// Bomb Projectile

class BombProjectile : public Projectile
{
private:
    sf::Sprite sprite;
    sf::Texture texture;
    float animTimer;
    int animFrame;
    bool hitGround;

    static const int BOMB_FRAMES = 14;
    static const sf::IntRect bombFrames[14];

public:
    BombProjectile(float x, float y, float vx, float vy, int damage, bool fromPlayer);


    void update(float dt);
    void update(float dt, const class World& world);
    void update(float dt, const class World& world, float playerX, float playerY);

   
    void render(sf::RenderWindow& window);

    void render(sf::RenderWindow& window, float camX, float camY);
    sf::FloatRect getBounds() const;
    bool isExplosive() const 

{

    return true;

}
};

// Grenade Projectile

class GrenadeProjectile : public Projectile
{
private:
    sf::Sprite sprite;
    sf::Texture texture;
    float animTimer;
    int animFrame;
    bool hitGround;

    static const int GRENADE_FRAMES = 5;
    static const sf::IntRect grenadeFrames[5];

public:
    GrenadeProjectile(float x, float y, float vx, float vy, int damage, bool fromPlayer);


    void update(float dt);
    void update(float dt, const class World& world);
    void update(float dt, const class World& world, float playerX, float playerY);

    void render(sf::RenderWindow& window);

    void render(sf::RenderWindow& window, float camX, float camY);
    sf::FloatRect getBounds() const;
    void onHit();
};

// Blast Projectile

class BlastProjectile : public Projectile
{
private:
    sf::Sprite sprite;
    sf::Texture texture;
    float animTimer;
    int animFrame;

    static const int BLAST_FRAMES = 12;
    static const sf::IntRect blastFrames[12];
public:
    BlastProjectile(float x, float y, bool fromPlayer = true);


    void update(float dt);

    void render(sf::RenderWindow& window);

    void render(sf::RenderWindow& window, float camX, float camY);
    sf::FloatRect getBounds() const;
    void onHit();
};

// Fire Pool Projectile

class FirePoolProjectile : public Projectile
{
private:
    sf::Sprite sprite;
    sf::Texture texture;
    float animTimer;
    int animFrame;
    float lifetime;
public:
    FirePoolProjectile(float x, float y);


    void update(float dt);

    void render(sf::RenderWindow& window);

    void render(sf::RenderWindow& window, float camX, float camY);
    sf::FloatRect getBounds() const;
    void onHit();
    bool isFlamePool() const 

{

    return true;

}
};
// M 15 A Rocket Projectile
class M15ARocketProjectile : public Projectile
{
private:
    sf::Sprite sprite;
    sf::Texture texture;
    float animTimer;
    int animFrame;
    bool isExploding;
    float explodeTimer;
    int explodeFrame;
    static const sf::IntRect rocketFrames[6];
    static const sf::IntRect explodeFrames[5];
public:
    M15ARocketProjectile(float x, float y, float vx, float vy);

    
    void update(float dt);
    void update(float dt, const class World& world);

    void render(sf::RenderWindow& window);


    void render(sf::RenderWindow& window, float camX, float camY);
    sf::FloatRect getBounds() const;
    void onHit();
};







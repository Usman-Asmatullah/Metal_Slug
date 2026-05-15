#pragma once
#include "Entities.h"
#include "Weapon_Types.h"
#include "World.h"
#include "Vehicle.h"
#include <cmath>

// Projectile

class Projectile;

// Soldier

class Soldier;

struct EnemyFrame {
    int x, y, w, h;
};

// Enemy

class Enemy : public DamagableEntity
{
protected:
    float velocityX, velocityY;
    float speed;
    bool facingRight;
    bool onGround;
    float fireTimer;
    float fireRate;
    float detectionRange;
    float animTimer;
    int animFrame;
    bool dying;
    float deathTimer;
    int deathFrame;

    sf::Texture texture;
    sf::Sprite sprite;

    static const float GRAVITY;

public:
    static const int TYPE_REBEL = 0;
    static const int TYPE_SHIELDED = 1;
    static const int TYPE_BAZOOKA = 2;
    static const int TYPE_GRENADE = 3;
    static const int TYPE_PARATROOPER = 4;
    static const int TYPE_MUMMY = 5;

    Enemy(float x, float y, int hp, float speed, float fireRate);
    virtual ~Enemy() 

{

}

    // Advances state calculations by frame time increment
    void update(float dt) 

{

    update(dt, 0, 0);

}
    virtual void update(float dt, float playerX, float playerY) = 0;
    virtual void render(sf::RenderWindow& window, float camX, float camY);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window) 

{

    render(window, 0, 0);

}
    virtual sf::FloatRect getBounds() const;
    virtual Projectile* shoot(float targetX, float targetY);
    void takeDamage(int amount);

    virtual int getType() const = 0;
    virtual int getScoreValue() const = 0;

    bool isOnGround() const 

{

    return onGround;

}
    void setOnGround(bool b) 

{

    onGround = b;

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
    bool isFacingRight() const 

{

    return facingRight;

}
    virtual bool isDying() const 

{

    return dying;

}
protected:
    void moveTowardPlayer(float playerX, float dt);
    float angleToTarget(float targetX, float targetY) const;
};

// Rebel Soldier

class RebelSoldier : public Enemy
{
public:
    RebelSoldier(float x, float y);
    void update(float dt, float playerX, float playerY);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return TYPE_REBEL;

}
    int getScoreValue() const 

{

    return 50;

}
    Projectile* shoot(float targetX, float targetY);
    sf::FloatRect getBounds() const;

private:
    sf::Texture rebelSheet;
    float stateTimer;       // patrol direction flip timer
    float shootCooldown;    // time until next shot attempt
    bool isFiring;         // true while playing fire animation
    bool shotFired;        // true if bullet already spawned this fire cycle
    bool pendingShoot;     // true when frame triggers a bullet spawn

    static const int WALK_FRAMES = 7;
    static const EnemyFrame walkFrames[7];

    static const int FIRE_FRAMES = 6;
    static const EnemyFrame fireFrames[6];

    static const float WALK_FRAME_SPEED;
    static const float FIRE_FRAME_SPEED;
    static const float SCALE;
};

// Shielded Soldier

class ShieldedSoldier : public Enemy
{
public:
    ShieldedSoldier(float x, float y);
    void update(float dt, float playerX, float playerY);

  
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return TYPE_SHIELDED;

}
    int getScoreValue() const 

{

    return 100;

}
    Projectile* shoot(float targetX, float targetY);
    sf::FloatRect getBounds() const;
    void takeDamage(int amount);

    bool isHitBlocked(float bulletX) const;

private:
    sf::Texture texture;
    float stateTimer;       
    float shootCooldown;    
    bool isFiring;         
    bool shotFired;        
    bool pendingShoot;     
    bool shieldActive;    

    static const int WALK_FRAMES = 6;
    static const EnemyFrame walkFrames[6];

    static const int FIRE_FRAMES = 10;
    static const EnemyFrame fireFrames[10];

    static const float WALK_FRAME_SPEED;
    static const float FIRE_FRAME_SPEED;
    static const float SCALE;
};

// Bazooka Soldier

class BazookaSoldier : public Enemy
{
public:
    BazookaSoldier(float x, float y);
    void update(float dt, float playerX, float playerY);

    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return TYPE_BAZOOKA;

}
    int getScoreValue() const 

{

    return 150;

}
    Projectile* shoot(float targetX, float targetY);
    sf::FloatRect getBounds() const;

private:
    sf::Texture texture;
    float stateTimer;      
    float shootCooldown;    
    bool isFiring;         
    bool shotFired;        
    bool pendingShoot;     

    // Parachute descend states
    bool isDescending;     
    float descendSpeed;    

    static const int SPAWN_FRAMES = 7;
    static const EnemyFrame spawnFrames[7];

    static const int HANGING_FRAMES = 2;
    static const EnemyFrame hangingFrames[2];

    static const int WALK_FRAMES = 6;
    static const EnemyFrame walkFrames[6];

    static const int FIRE_FRAMES = 3;
    static const EnemyFrame fireFrames[3];

    static const float WALK_FRAME_SPEED;
    static const float FIRE_FRAME_SPEED;
    static const float SPAWN_FRAME_SPEED;
    static const float SCALE;
};

// Grenade Thrower

class GrenadeThrower : public Enemy
{
public:
    GrenadeThrower(float x, float y);
    void update(float dt, float playerX, float playerY);

   
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return TYPE_GRENADE;

}
    int getScoreValue() const 

{

    return 120;

}
    Projectile* shoot(float targetX, float targetY);
    sf::FloatRect getBounds() const;

private:
    sf::Texture texture;
    float stateTimer;       
    float shootCooldown;   
    bool isFiring;        
    bool shotFired;        
    bool pendingShoot;     

    static const int WALK_FRAMES = 1;
    static const EnemyFrame walkFrames[1];

    static const int FIRE_FRAMES = 8;
    static const EnemyFrame fireFrames[8];

    static const float WALK_FRAME_SPEED;
    static const float FIRE_FRAME_SPEED;
    static const float SCALE;
};

// Paratrooper

class Paratrooper : public Enemy
{
public:
    Paratrooper(float x, float y);
    void update(float dt, float playerX, float playerY);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return TYPE_PARATROOPER;

}
    int getScoreValue() const 

{

    return 100;

}
    sf::FloatRect getBounds() const;

private:
    sf::Texture texture;
    bool isDescending;
    float descendSpeed;

    static const int SPAWN_FRAMES = 7;
    static const EnemyFrame spawnFrames[7];

    static const int HANGING_FRAMES = 2;
    static const EnemyFrame hangingFrames[2];

    static const float SPAWN_FRAME_SPEED;
    static const float SCALE;
};

// Mummy Warrior

class MummyWarrior : public Enemy
{
public:
    MummyWarrior(float x, float y);
    void update(float dt, float playerX, float playerY);

   
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return TYPE_MUMMY;

}
    int getScoreValue() const 

{

    return 150;

}
    sf::FloatRect getBounds() const;
    void takeDamage(int amount);

    bool isCrumbling() const 

{

    return crumbling;

}
private:
    float stateTimer;
    bool crumbling;
    float crumbleTimer;

    static const int WALK_FRAMES = 8;
    static const EnemyFrame walkFrames[8];

    static const int CRUMBLE_FRAMES = 7;
    static const EnemyFrame crumbleFrames[7];

    static const int DEATH_FRAMES = 11;
    static const EnemyFrame deathFrames[11];

    static const float WALK_FRAME_SPEED;
    static const float CRUMBLE_FRAME_SPEED;
    static const float SCALE;
};

// Zombie Enemy

class ZombieEnemy : public Enemy
{
public:
    ZombieEnemy(float x, float y);
    void update(float dt, float playerX, float playerY);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return 6;

}
    int getScoreValue() const 

{

    return 250;

}
    sf::FloatRect getBounds() const;
private:
    float stateTimer;
    static const int WALK_FRAMES = 4;
    static const float WALK_FRAME_SPEED;
    static const float SCALE;
};

// Flying Tara

class FlyingTara : public Enemy, public AerialVehicle
{
public:
    FlyingTara(float x, float y);
    void update(float dt, float playerX, float playerY);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return 7;

}
    int getScoreValue() const 

{

    return 300;

}
    sf::FloatRect getBounds() const;
    Projectile* shoot(float targetX, float targetY);
    void takeDamage(int amount);
    virtual void handleInput(float dt) { }

    // Executes weapon discharge spawning new projectile
    virtual void fire(Projectile*& p) { }

    
    virtual void onDestroyed() { }

    

    void update(float dt) 

{

    Enemy::update(dt);

}

    void render(sf::RenderWindow& w) 

{

    Enemy::render(w);

}
private:
    float shootCooldown;
    static const float SCALE;
    static const EnemyFrame moveFrames[3];
};

// Alien Enemy

class AlienEnemy : public Enemy
{
public:
    AlienEnemy(float x, float y);
    void update(float dt, float playerX, float playerY);

 
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return 8;

}
    int getScoreValue() const 

{

    return 400;

}
    sf::FloatRect getBounds() const;
    Projectile* shoot(float targetX, float targetY);
private:
    float shootCooldown;
    static const float SCALE;
};

// Aerial Enemy

class AerialEnemy : public Enemy
{
public:
    AerialEnemy(float x, float y);
    void update(float dt, float playerX, float playerY);

    
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return 9;

}
    int getScoreValue() const 

{

    return 350;

}
    sf::FloatRect getBounds() const;
    Projectile* shoot(float targetX, float targetY);
private:
    float shootCooldown;
    static const float SCALE;
};
// M 15
class M15ABradley : public Enemy, public GroundVehicle
{
public:
    M15ABradley(float x, float y);
    void update(float dt, float playerX, float playerY);

    
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return 10;

}
    int getScoreValue() const 

{

    return 1000;

}
    sf::FloatRect getBounds() const;
    void takeDamage(int amount);
    virtual void handleInput(float dt) { }

    virtual void fire(Projectile*& p) { }

    virtual void onDestroyed() { }

    void update(float dt) 

{

    Enemy::update(dt);

}
    void render(sf::RenderWindow& w) 

{

    Enemy::render(w);

}
private:
    bool loadingRocket;
    float loadTimer;
    int loadFrame;
    bool shotFired;

    static const int MOVE_FRAMES = 6;
    static const EnemyFrame moveFrames[6];

    static const int LOAD_FRAMES = 6;
    static const EnemyFrame loadFrames[6];

    static const int DEATH_FRAMES = 6;
    static const EnemyFrame deathFrames[6];

    static const float SCALE;
};

// Enemy Sub

class EnemySub : public Enemy
{
public:
    EnemySub(float x, float y);
    void update(float dt, float playerX, float playerY);

    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return 11;

}
    int getScoreValue() const 

{

    return 800;

}
    sf::FloatRect getBounds() const;
    Projectile* shoot(float targetX, float targetY);
private:
    float shootCooldown;
    static const float SCALE;
    static const EnemyFrame moveFrames[4];
};

// P O W Prisoner

class POWPrisoner : public Enemy
{
public:
    POWPrisoner(float x, float y);
    void update(float dt, float playerX, float playerY);

    // Draws current frame buffer pixels to visual display
    void render(sf::RenderWindow& window, float camX, float camY);
    int getType() const 

{

    return 12;

}
    int getScoreValue() const 

{

    return 500;

}
    sf::FloatRect getBounds() const;
    bool isRescued() const 

{

    return rescued;

}
    void rescue()              

{

    rescued = true;

}
private:
    bool rescued;
    static const float SCALE;
};

// Enemy Manager

class EnemyManager
{
private:
    static const int MAX_ENEMIES = 128;
    static const int MAX_ENEMY_PROJECTILES = 256;

    Enemy* enemies[MAX_ENEMIES];
    int count;
    Projectile* projectiles[MAX_ENEMY_PROJECTILES];
    int projCount;

    bool firstSpawnDone;
    int spawnWaves;        // how many waves have spawned (max 2)
    float respawnTimer;      // counts up to 15 seconds for second wave
    float m15aSpawnTimer;
    bool m15aSpawned;
    float flyingTaraSpawnTimer;
    bool flyingTaraSpawned;

    void resolveEnemyH(Enemy& e, const World& w);
    void resolveEnemyV(Enemy& e, const World& w);
    float findGroundY(int tileCol, float fallback, const World& world) const;

public:
    EnemyManager();
    ~EnemyManager();

    EnemyManager(const EnemyManager&) = delete;
    EnemyManager& operator=(const EnemyManager&) = delete;
    EnemyManager(EnemyManager&& other) noexcept;
    EnemyManager& operator=(EnemyManager&& other) noexcept;

    void clear();
    void spawnEnemy(Enemy* e);
    void addProjectile(Projectile* p);
    void update(float dt, Soldier* player, const World& world);
    void updateLevel2(float dt, Soldier* player, const World& world);
    void updateLevel3(float dt, Soldier* player, const World& world);
    void updateDynamicLevel(float dt, Soldier* player, const World& world, struct LevelSpawn* spawns, int spawnCount);


    void render(sf::RenderWindow& window, float camX, float camY);

    Projectile** getProjectiles() 

{

    return projectiles;

}
    int getProjectileCount() const 

{

    return projCount;

}
    int getEnemyCount() const 

{

    return count;

}
    int getSpawnWaves() const 

{

    return spawnWaves;

}
    Enemy* getEnemy(int i) 

{

    return enemies[i];

}
    void removeDeadEnemies();
    void removeDeadProjectiles();
};

void registerPlayerProjectile(class Projectile* p);









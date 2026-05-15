
#pragma once
#include "Weapon.h"       
#include "Projectile.h"  

// Pistol

class Pistol : public Weapon
{
public:
    Pistol()
    

{
        weaponType = TYPE_PISTOL;
        damage = 3;
        fireRate = 2;
        fireTimer = 0;
        ammo = 10000;
        infiniteAmmo = true;
    }

    void fire(float x, float y, float angle, Projectile*& projectiles)
    

{
        float speed = 600;
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;

        projectiles = new StraightProjectile(x, y, vx, vy, damage, true,
            sf::Color::Green,              // pistol bullets are green
            8, 4
        );

        fireTimer = 0.5f; // reload/cooldown time of 0.5 seconds
    }

};

// Heavy Machine Gun

class HeavyMachineGun : public Weapon
{
public:
    HeavyMachineGun()
    

{
        weaponType = TYPE_HMG;
        damage = 2;
        fireRate = 20;
        fireTimer = 0;
        ammo = 99999;
        infiniteAmmo = true;  // infinite ammo
    }

    void fire(float x, float y, float angle, Projectile*& projectiles)
    

{
        float speed = 700;
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;
void registerPlayerProjectile(Projectile * p);
        float baseY = y + 10.f; // lowered the bullets slightly

        // First bullet (center)
        projectiles = new StraightProjectile(x, baseY, vx, vy, damage, true, sf::Color(255, 140, 0), 7, 3);

        // Second bullet (slightly higher)
        Projectile* p2 = new StraightProjectile(x, baseY - 10.f, vx, vy, damage, true, sf::Color(255, 140, 0), 7, 3);
        registerPlayerProjectile(p2);

        // Third bullet (slightly lower)
        Projectile* p3 = new StraightProjectile(x, baseY + 10.f, vx, vy, damage, true, sf::Color(255, 140, 0), 7, 3);
        registerPlayerProjectile(p3);

        // Fourth bullet (even higher)
        Projectile* p4 = new StraightProjectile(x, baseY - 20.f, vx, vy, damage, true, sf::Color(255, 140, 0), 7, 3);
        registerPlayerProjectile(p4);

        fireTimer = 1.0 / fireRate;
        ammo--;
    }

    int getDamage()
    

{
    

    return damage;
    

}
};

// Rocket Launcher

class RocketLauncher : public Weapon
{
private:
    float blastRadius;


public:
    RocketLauncher()
    

{
        weaponType = TYPE_ROCKET;
        damage = 5;
        fireRate = 0.5;
        fireTimer = 0;
        ammo = 5;
        infiniteAmmo = false;
        blastRadius = 200;
    }

    void fire(float x, float y, float angle, Projectile*& projectiles)
    

{

        float speed = 400;
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;

        projectiles = new BallisticProjectile(x, y, vx, vy, damage, true, blastRadius, true);


        fireTimer = 1 / fireRate;
        ammo--;
    }



    float getBlastRadius()
    

{
    

    return blastRadius;
    

}
};

// Flame Shot

class FlameShot : public Weapon
{
private:
    bool isFiring;
    float flame_length;

public:
    FlameShot()
    

{
        weaponType = TYPE_FLAME;
        damage = 2;
        fireRate = 20;
        fireTimer = 0;
        ammo = 230;
        infiniteAmmo = false;
        isFiring = false;
        flame_length = 320;
    }

    void fire(float x, float y, float angle, Projectile*& projectiles)
    

{
        float speed = 300;
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;


        projectiles = new FlameProjectile(x, y, vx, vy, true);

        isFiring = true;
        fireTimer = 1 / fireRate;
        ammo--;
    }

    void stopFiring()
    

{
    

    isFiring = false;
    

}
    bool getIsFiring()
    

{
    

    return isFiring;
    

}
    float getLength() const
    

{
    

    return flame_length;
    

}
};

// Laser Gun

class LaserGun : public Weapon
{
public:
    LaserGun()
    

{
        weaponType = TYPE_LASER;
        damage = 10000;       // instant kill 
        fireRate = 1;
        fireTimer = 0;
        ammo = 10;
        infiniteAmmo = false;
    }

    void fire(float x, float y, float angle, Projectile*& projectiles)
    

{

        bool facingRight = (cos(angle) >= 0);//facing right if cos is positive
        projectiles = new LaserProjectile(x, y, 1600, facingRight, true);

        fireTimer = 1 / fireRate;
        ammo--;
    }

    int getDamage()
    

{
    

    return damage;
    

}
};

// Knife

class Knife
{
private:
    int damage;
    float cooldown;
    float cooldownTimer;

public:
    Knife()
        : damage(2), cooldown(0.5f), cooldownTimer(0)
    

{
    

}

    // Advances state calculations by frame time increment
    void update(float dt)
    

{
        if (cooldownTimer > 0)
            cooldownTimer = cooldownTimer - dt;
    }


    bool canSwing() const
    

{
    

    return cooldownTimer <= 0;
    

}
    void swing()
    

{  ///APPLY LOGIC..............................................
    }

    int getDamage() const
    

{
    

    return damage;
    

}
};

// Grenade

class Grenade
{
protected:
    int damage;
    float blastRadius;

public:
    Grenade() : damage(20), blastRadius(192)
    

{
    

}
    virtual ~Grenade() 

{

}
    virtual Projectile* throwGrenade(float x, float y, float angle)
    

{
        float speed = 500;
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;


        return new BallisticProjectile(x, y, vx, vy, damage, true, blastRadius, true, sf::Color(150, 150, 150), false);
    }

    int getDamage() const
    

{
    

    return damage;
    

}
    float getBlastRadius() const
    

{
    

    return blastRadius;
    

}
};

// Fire Bomb Grenade

class FireBombGrenade : public Grenade
{
private:
    float firePoolDuration;
    float firePoolRadius;

public:
    FireBombGrenade()
    

{
        damage = 20;
        blastRadius = 192;
        firePoolDuration = 10;
        firePoolRadius = 192;
    }

    Projectile* throwGrenade(float x, float y, float angle)
    

{
        float speed = 500;
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;
        return new BallisticProjectile(x, y, vx, vy, damage, true, blastRadius, true, sf::Color(255, 140, 0), true);
    }

    float getFirePoolDuration() const
    

{
    

    return firePoolDuration;
    

}
    float getFirePoolRadius() const
    

{
    

    return firePoolRadius;
    

}
};







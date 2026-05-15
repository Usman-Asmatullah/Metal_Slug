#pragma once
#include "Projectile.h"

// Weapon

class Weapon
{
protected:
    int weaponType;
    int ammo;
    bool infiniteAmmo;
    float fireRate, fireTimer;
    int damage;
public:
    static const int TYPE_PISTOL = 0;
    static const int TYPE_HMG = 1;
    static const int TYPE_ROCKET = 2;
    static const int TYPE_FLAME = 3;
    static const int TYPE_LASER = 4;


   

    virtual void fire(float x, float y, float angle, Projectile*& projectiles) = 0;
    virtual void reload();
    bool canFire();
    int getAmmo();
    int getWeaponType() const
    

{
    

    return weaponType;
    

}
    void setAmmo(int a) 

{ ammo = a; }   // needed by Fio::pickupWeapon
    virtual void update(float dt);
    virtual ~Weapon() 

{

}
};

// Projectile Weapon

class ProjectileWeapon : public Weapon {
};







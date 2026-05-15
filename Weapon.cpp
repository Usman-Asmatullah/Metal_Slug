// Weapon.cpp
#include "Weapon.h"



bool Weapon::canFire()


{
    if (fireTimer <= 0 && (infiniteAmmo || ammo > 0))
    

{
    

    return true;
    

}
    return false;
}

int Weapon::getAmmo()


{


    return ammo;


}
void Weapon::reload()


{


    fireTimer = 1 / fireRate;


}

void Weapon::update(float dt)


{
    if (fireTimer > 0)
        fireTimer = fireTimer - dt;
}




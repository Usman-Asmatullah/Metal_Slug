


#include "libraries.h"
#include "Entities.h"

int Entity::nextID = 0;


Entity::Entity(float x, float y) : x(x), y(y), alive(true), id(nextID)


{


    nextID++;


}
const float DamagableEntity::damagedStateDuration = 0.3;



DamagableEntity::DamagableEntity(float x, float y, int maxHP) : Entity(x, y), Hp(maxHP), maxHP(maxHP), DamageState(normalState), damageTimer(0)


{


}
void DamagableEntity::takeDamage(int amount)


{

    if (DamageState == Dead)
    

{
    

    return;
    

}
    Hp = Hp - amount;


    damageTimer = damagedStateDuration;   // reset to 0.3s


    if (Hp <= 0)
    

{
        //entity is dead
        Hp = 0;
        DamageState = Dead;
        alive = false;   // inherited from Entity


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


void DamagableEntity::heal(int amount)


{

    Hp = Hp + amount;//increase health

    if (Hp > maxHP)//cap
    {
        Hp = maxHP;
    }
    if (Hp >= maxHP / 3)
    

{
    

    DamageState = normalState;
    

}
}


void DamagableEntity::updateDamageState(float dt)


{
    if (damageTimer > 0)
        damageTimer = damageTimer - dt;

}



void DamagableEntity::draw_damage(sf::RenderWindow& window, const sf::FloatRect& bounds) const


{


    if (DamageState == normalState)   
    {
        return;
    }
    sf::RectangleShape draw_damage(MyVector2f(bounds.width, bounds.height));
    draw_damage.setPosition(bounds.left, bounds.top);

    if (DamageState == injuredState)
        draw_damage.setFillColor(sf::Color(255, 0, 0, 80));//Light red


    else if (DamageState == criticalState)
        draw_damage.setFillColor(sf::Color(255, 0, 0, 160));//Full red


    window.draw(draw_damage);
}








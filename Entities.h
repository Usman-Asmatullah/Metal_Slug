#pragma once
#include <SFML/Graphics.hpp>

// Entity

class Entity
{
protected:
    float x, y;
    bool alive;
    int id;

private:
    static int nextID;

public:
    Entity(float x = 0, float y = 0);
    virtual ~Entity() = default;

    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    virtual sf::FloatRect getBounds() const = 0;

    float getX() const 

{

    return x;

}
    float getY() const 

{

    return y;

}
    void setPosition(float x1, float y1) 

{

    this->x = x1;
    this->y = y1;

}
    void move(float dx, float dy) 

{

    x += dx;
    y += dy;

}
    bool isAlive() const 

{

    return alive;

}
    void setAlive(bool aliveA) 

{

    alive = aliveA;

}
    int getID() const 

{

    return id;

}
};

// Damagable Entity

class DamagableEntity : public Entity
{
public:

    static const int normalState = 0;
    static const int injuredState = 1;
    static const int criticalState = 2;
    static const int Dead = 3;


    DamagableEntity(float x, float y, int maxHP);
    virtual ~DamagableEntity() = default;

    virtual void takeDamage(int amount);

    virtual void heal(int amount);

    int getHP() const 

{

    return (Hp + 2) / 3;

}
    int getMaxHP() const 

{

    return maxHP;

}
    bool isDead() const 

{

    return DamageState == Dead;

}
    int getDamageState() const 

{

    return DamageState;

}
    virtual void updateDamageState(float dt);

protected:
    int Hp;
    int maxHP;
    int DamageState;
    float damageTimer;
    static const float damagedStateDuration;


    void draw_damage(sf::RenderWindow& window, const sf::FloatRect& bounds) const;
};






#include "Projectile.h"
#include "World.h"
#include "CustomBounds.h"
#include "Enemy.h"

void Projectile::render(sf::RenderWindow& window)


{


    render(window, 0, 0);


}

void Projectile::render(sf::RenderWindow& window, float camX, float camY)


{


}
sf::FloatRect Projectile::getBounds() const


{


    return sf::FloatRect(x, y, 1, 1);


}
StraightProjectile::StraightProjectile(
    float x, float y, float vx, float vy,
    int damage, bool fromPlayer,
    sf::Color color,
    float width, float height)
    : Projectile(x, y, vx, vy, damage, fromPlayer)


{
    shape.setSize({ width, height });
    shape.setFillColor(color);
    shape.setOrigin(width / 2, height / 2);
    shape.setPosition(x, y);
}


// Advances state calculations by frame time increment

void StraightProjectile::update(float dt)


{
    x = x + (velx * dt);
    y = y + (vely * dt);

    shape.setPosition(x, y);

    if (x < -10000.f || x > 100000.f || y < -2000.f || y > 3000.f)
    

{
    

    alive = false;
    

}
}

void StraightProjectile::update(float dt, const class World& world)


{
    update(dt);
    int tileCol = (int)(x / 32.f);
    int tileRow = (int)(y / 32.f);
    if (world.isSolid(tileCol, tileRow))
    

{
    

    alive = false;
    

}
}



void StraightProjectile::render(sf::RenderWindow& window)


{


    render(window, 0, 0);


}

void StraightProjectile::render(sf::RenderWindow& window, float camX, float camY)


{
    if (alive)
    

{
        shape.setPosition(x - camX, y - camY);
        window.draw(shape);
    }
}

sf::FloatRect StraightProjectile::getBounds() const


{


    return ::getGlobalBounds(shape);


}
const float BallisticProjectile::GRAVITY = 500;

BallisticProjectile::BallisticProjectile(float x, float y, float vx, float vy, int damage, bool fromPlayer, float blastRadius, bool explosive, sf::Color color, bool isFire)
    : Projectile(x, y, vx, vy, damage, fromPlayer), blastRadius(blastRadius), explosive(explosive), isFire(isFire)


{
    float radius;
    if (explosive)
    
{
    
    radius = 6;
    
}
    else
    
{
    
    radius = 4;
    
}
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setFillColor(color);
    shape.setPosition(x, y);
}



void BallisticProjectile::update(float dt)


{

    vely = vely + (GRAVITY * dt);


    x = x + (velx * dt);
    y = y + (vely * dt);

    shape.setPosition(x, y);

    if (x < -10000.f || x > 100000.f || y > 3000.f)
    

{
    

    alive = false;
    

}
}

void BallisticProjectile::update(float dt, const class World& world)


{
    update(dt);

    // Check ground collision
    int tileCol = (int)(x / 32.f);
    int tileRow = (int)(y / 32.f);
    if (world.isSolid(tileCol, tileRow) || y >= 1500.f)
    

{
    

    onHit();
    

}
}

void BallisticProjectile::update(float dt, const class World& world, float playerX, float playerY)


{


    update(dt, world);


}
void BallisticProjectile::onHit()


{
    if (alive)
    

{
        alive = false;

        if (!isFire)
        

{
        

    void registerPlayerProjectile(Projectile * p);
    registerPlayerProjectile(new BlastProjectile(x, y));
        

}
        else
        

{
    void registerPlayerProjectile(Projectile * p);
    registerPlayerProjectile(new FirePoolProjectile(x, y));
        }
    }
}


void BallisticProjectile::render(sf::RenderWindow& window)


{


    render(window, 0, 0);


}

void BallisticProjectile::render(sf::RenderWindow& window, float camX, float camY)


{
    if (alive)
    

{
        shape.setPosition(x - camX, y - camY);
        window.draw(shape);
    }
}

sf::FloatRect BallisticProjectile::getBounds() const


{


    return ::getGlobalBounds(shape);


}
FlameProjectile::FlameProjectile
(float x, float y, float vx, float vy, bool fromPlayer) : StraightProjectile(x, y, vx, vy, 5, fromPlayer,
    sf::Color(255, 80, 0),   // orange-red 
    16, 12)               
{
}


void FlameProjectile::update(float dt)


{
    StraightProjectile::update(dt);


    sf::Color c = shape.getFillColor();      // shape is from StraightProjectile
    if (c.a > 8)
        c.a = c.a - static_cast<unsigned char>(255 * dt * 2);  // fade to transparent in milisecond
    else
    

{
        c.a = 0;
        alive = false;
    }
    shape.setFillColor(c);
}


void FlameProjectile::render(sf::RenderWindow& window)


{


}
void FlameProjectile::render(sf::RenderWindow& window, float camX, float camY)


{


}
LaserProjectile::LaserProjectile(
    float originX, float originY,
    float length, bool facingRight, bool fromPlayer)
    : Projectile(originX, originY, 0, 0, 0, fromPlayer),
    length(length), facingRight(facingRight)


{
    beam.setSize({ length, 4.f });
    beam.setFillColor(sf::Color(255, 0, 0, 220));

    if (facingRight)
        beam.setOrigin(0, 2);
    else
        beam.setOrigin(length, 2);

    beam.setPosition(originX, originY);
}




void LaserProjectile::update(float dt)


{
    lifetime = lifetime - dt;

    if (lifetime <= 0)
        alive = false;

}


void LaserProjectile::render(sf::RenderWindow& window)


{


    render(window, 0, 0);


}


void LaserProjectile::render(sf::RenderWindow& window, float camX, float camY)


{

    if (alive)
    

{
        sf::Color c = beam.getFillColor();
        c.a = static_cast<unsigned char>((lifetime / 0.1) * 220);
        beam.setFillColor(c);
        beam.setPosition(x - camX, y - camY);
        window.draw(beam);
    }
}

sf::FloatRect LaserProjectile::getBounds() const


{


    return ::getGlobalBounds(beam);


}
// BombProjectile Implementation
const sf::IntRect BombProjectile::bombFrames[14] = {
    {136, 770, 28, 27},
    {33, 773, 23, 23},
    {59, 774, 21, 21},
    {83, 775, 23, 20},
    {109, 772, 24, 24},
    {136, 770, 28, 27},
    {167, 768, 28, 29},
    {198, 768, 32, 29},
    {233, 767, 34, 30},
    {270, 766, 34, 30},
    {307, 766, 35, 29},
    {345, 765, 34, 30},
    {382, 765, 36, 30},
    {421, 765, 30, 22}
};

BombProjectile::BombProjectile(float x, float y, float vx, float vy, int damage, bool fromPlayer)
    : Projectile(x, y, vx, vy, damage, fromPlayer), animTimer(0), animFrame(0), hitGround(false)


{
    if (texture.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png"))
    

{
        sprite.setTexture(texture);
        sprite.setTextureRect(bombFrames[0]);
        sprite.setScale(3.f, 3.f);
        sprite.setOrigin((float)bombFrames[0].width / 2.f, (float)bombFrames[0].height / 2.f);
    }
}




void BombProjectile::update(float dt)


{
}

void BombProjectile::update(float dt, const class World& world)


{


    update(dt, world, -99999.f, -99999.f);


}
void BombProjectile::update(float dt, const class World& world, float playerX, float playerY)


{
    if (!hitGround)
    

{
        vely += 120.f * dt;

        x += velx * dt;
        y += vely * dt;

        float playerLeft = playerX;
        float playerRight = playerX + 90.f;
        float playerTop = playerY;
        float playerBottom = playerY + 145.f;

        float closestX = x;
        if (closestX < playerLeft)
        {
            closestX = playerLeft;
        }
        if (closestX > playerRight)
        {
            closestX = playerRight;
        }

        float closestY = y;
        if (closestY < playerTop)
        {
            closestY = playerTop;
        }
        if (closestY > playerBottom)
        {
            closestY = playerBottom;
        }

        float dx = closestX - x;
        float dy = closestY - y;
        float distSq = dx * dx + dy * dy;

        if (playerX > -90000.f && distSq < 50.f * 50.f)
        

{
            hitGround = true;
            velx = 0;
            vely = 0;
            animFrame = 5; 
            animTimer = 0;
        }
        else
        

{
            // Check ground collision 
            int tileCol = (int)(x / CELL);
            int tileRow = (int)(y / CELL);
            if (world.isSolid(tileCol, tileRow))
            

{
                hitGround = true;
                velx = 0;
                vely = 0;
                y = (float)(tileRow * CELL) - 5.f; // Place slightly above to avoid sinking
                animFrame = 5; 
                animTimer = 0;
            }
        }

        animTimer += dt;
        if (animTimer >= 0.08f)
        

{
            animTimer = 0;
            animFrame = (animFrame + 1) % BOMB_FRAMES;
        }
    }
    else
    

{
        animTimer += dt;
        if (animTimer >= 0.08f)
        

{
            animTimer = 0;
            animFrame++;
            if (animFrame >= BOMB_FRAMES)
            

{
            

    alive = false;
            

}
        }
    }

    if (x < -10000.f || x > 100000.f || y > 3000.f || y < -2000.f)
    

{
    

    alive = false;
    

}
}



void BombProjectile::render(sf::RenderWindow& window)


{


    render(window, 0, 0);


}

void BombProjectile::render(sf::RenderWindow& window, float camX, float camY)


{
    if (alive)
    

{
        const sf::IntRect& r = bombFrames[animFrame % BOMB_FRAMES];
        sprite.setTextureRect(r);
        sprite.setOrigin((float)r.width / 2.f, (float)r.height / 2.f);
        sprite.setPosition(x - camX, y - camY);
        window.draw(sprite);
    }
}

sf::FloatRect BombProjectile::getBounds() const


{
    const sf::IntRect& r = bombFrames[animFrame % BOMB_FRAMES];
    return sf::FloatRect(x - (float)r.width * 1.5f, y - (float)r.height * 1.5f, (float)r.width * 3.f, (float)r.height * 3.f);
}

// GrenadeProjectile Implementation
const sf::IntRect GrenadeProjectile::grenadeFrames[5] = {
    {79, 806, 10, 10},
    {66, 806, 10, 10},
    {53, 806, 10, 10},
    {40, 806, 10, 10},
    {18, 812, 1, 1}
};

GrenadeProjectile::GrenadeProjectile(float x, float y, float vx, float vy, int damage, bool fromPlayer)
    : Projectile(x, y, vx, vy, damage, fromPlayer), animTimer(0), animFrame(0), hitGround(false)


{
    if (texture.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png"))
    

{
        sprite.setTexture(texture);
        sprite.setTextureRect(grenadeFrames[0]);
        sprite.setScale(3.f, 3.f);
        sprite.setOrigin((float)grenadeFrames[0].width / 2.f, (float)grenadeFrames[0].height / 2.f);
    }
}



void GrenadeProjectile::update(float dt) 

{

}
void GrenadeProjectile::update(float dt, const class World& world) 

{

    update(dt, world, -99999.f, -99999.f);

}
void GrenadeProjectile::update(float dt, const class World& world, float playerX, float playerY)


{
    if (!hitGround)
    

{
        // Ballistic arc physics
        vely += 600.f * dt;

        x += velx * dt;
        y += vely * dt;

        int tileCol = (int)(x / CELL);
        int tileRow = (int)(y / CELL);
        if (world.isSolid(tileCol, tileRow))
        

{
            hitGround = true;
            velx = 0;
            vely = 0;
            animFrame = 4;
            animTimer = 0;
        }

        animTimer += dt;
        if (animTimer >= 0.08f)
        

{
            animTimer = 0;
            animFrame = (animFrame + 1) % 4;
        }
    }
    else
    

{
    

    onHit();
    

}
    if (x < -10000.f || x > 100000.f || y > 3000.f || y < -2000.f)
    

{
    

    alive = false;
    

}
}



void GrenadeProjectile::render(sf::RenderWindow& window) 

{

    render(window, 0, 0);

}

void GrenadeProjectile::render(sf::RenderWindow& window, float camX, float camY)


{
    if (alive)
    

{
        const sf::IntRect& r = grenadeFrames[animFrame % GRENADE_FRAMES];
        sprite.setTextureRect(r);
        sprite.setOrigin((float)r.width / 2.f, (float)r.height / 2.f);
        sprite.setPosition(x - camX, y - camY);
        window.draw(sprite);
    }
}

sf::FloatRect GrenadeProjectile::getBounds() const


{


    return sf::FloatRect(x - 25.f, y - 25.f, 50.f, 50.f);


}
void GrenadeProjectile::onHit()


{
    if (alive)
    

{
        alive = false;
        if (fromPlayer)
        

{
        

    void registerPlayerProjectile(Projectile * p);
    registerPlayerProjectile(new BlastProjectile(x, y, true));
        

}
        else
        

{
            if (GameData::activeEnemyManager)
            

{
            

    GameData::activeEnemyManager->addProjectile(new BlastProjectile(x, y, false));
            

}
        }
    }
}

const sf::IntRect BlastProjectile::blastFrames[12] = {
    {15, 690, 32, 28},
    {0, 0, 91, 73},
    {112, 0, 104, 80},
    {240, 0, 115, 83},
    {357, 0, 117, 81},
    {476, 0, 244, 80},
    {0, 75, 122, 93},
    {136, 87, 107, 81},
    {264, 88, 104, 82},
    {392, 88, 96, 80},
    {520, 88, 88, 72},
    {656, 88, 72, 56}
};

BlastProjectile::BlastProjectile(float x, float y, bool fromPlayer)
    : Projectile(x, y, 0, 0, 999, fromPlayer), animTimer(0), animFrame(0)


{
    if (texture.loadFromFile("Sprites/Player/Bomb Blast.png"))
    

{
        sprite.setTexture(texture);
        sprite.setTextureRect(blastFrames[0]);
        sprite.setScale(1.5f, 1.5f);
    }
}




void BlastProjectile::update(float dt)


{
    animTimer += dt;
    if (animTimer >= 0.05f)
    

{
        animTimer = 0;
        animFrame++;
        if (animFrame >= BLAST_FRAMES)
        

{
        

    alive = false;
        

}
    }
}


// Draws current frame buffer pixels to visual display

void BlastProjectile::render(sf::RenderWindow& window) 

{

    render(window, 0, 0);

}

// Draws current frame buffer pixels to visual display
void BlastProjectile::render(sf::RenderWindow& window, float camX, float camY)


{
    if (alive && animFrame < BLAST_FRAMES)
    

{
        sf::IntRect rect = blastFrames[animFrame];
        sprite.setTextureRect(rect);
        sprite.setOrigin((float)rect.width / 2.f, (float)rect.height);
        sprite.setPosition(x - camX, y - camY + 20.f);
        window.draw(sprite);
    }
}

sf::FloatRect BlastProjectile::getBounds() const


{


    return sf::FloatRect(x - 144.f, y - 144.f, 288.f, 288.f);


}
void BlastProjectile::onHit()


{


}
FirePoolProjectile::FirePoolProjectile(float x, float y)
    : Projectile(x, y, 0, 0, 5, true), animTimer(0), animFrame(0), lifetime(10.f)


{
    if (texture.loadFromFile("Sprites/Player/flame.png"))
    

{
        sprite.setTexture(texture);
        sprite.setScale(3.f, 3.f);
    }
}


// Advances state calculations by frame time increment

void FirePoolProjectile::update(float dt)


{
    lifetime -= dt;
    if (lifetime <= 0)
    

{
        alive = false;
        return;
    }

    animTimer += dt;
    if (animTimer >= 0.08f)
    

{
        animTimer = 0;
        animFrame = (animFrame + 1) % 8;
    }
}



void FirePoolProjectile::render(sf::RenderWindow& window) 

{

    render(window, 0, 0);

}

void FirePoolProjectile::render(sf::RenderWindow& window, float camX, float camY)


{
    if (alive)
    

{
        int frameW = texture.getSize().x / 8;
        if (frameW <= 0)
        {
            frameW = 32;
        }
        int frameH = texture.getSize().y;
        sprite.setTextureRect(sf::IntRect(animFrame * frameW, 0, frameW, frameH));
        sprite.setOrigin((float)frameW / 2.f, (float)frameH);

        for (int i = -1; i <= 1; i++)
        

{
            sprite.setPosition(x + i * 64.f - camX, y - camY + 20.f);
            window.draw(sprite);
        }
    }
}

sf::FloatRect FirePoolProjectile::getBounds() const


{
    return sf::FloatRect(x - 120.f, y - 172.f, 240.f, 192.f);
}

void FirePoolProjectile::onHit()


{


}
// M15ARocketProjectile Implementation
const sf::IntRect M15ARocketProjectile::rocketFrames[6] = {
    {183, 4142, 50, 18},
    {235, 4143, 50, 15},
    {287, 4143, 50, 14},
    {339, 4143, 50, 14},
    {391, 4141, 49, 18},
    {3, 4173, 42, 36}
};

const sf::IntRect M15ARocketProjectile::explodeFrames[5] = {
    {240, 4064, 60, 30},
    {303, 4062, 64, 33},
    {11, 4098, 52, 30},
    {138, 4098, 46, 28},
    {404, 4098, 22, 29}
};

M15ARocketProjectile::M15ARocketProjectile(float x, float y, float vx, float vy)
    : Projectile(x, y, vx, vy, 2, false), animTimer(0), animFrame(0), isExploding(false), explodeTimer(0), explodeFrame(0)


{
    if (!texture.loadFromFile("Sprites/Enemies/Rocket.png"))
    

{
    

    texture.loadFromFile("Sprites/Enemies/Rebel Soldier (Bazooka).png");
    

}
    sprite.setTexture(texture);
    sprite.setTextureRect(rocketFrames[0]);
    sprite.setScale(2.5f, 2.5f);
    sprite.setOrigin((float)rocketFrames[0].width / 2.f, (float)rocketFrames[0].height / 2.f);
}


// Advances state calculations by frame time increment

void M15ARocketProjectile::update(float dt)


{
    if (isExploding)
    

{
        explodeTimer += dt;
        if (explodeTimer >= 0.08f)
        

{
            explodeTimer = 0;
            explodeFrame++;
            if (explodeFrame >= 5)
            

{
            

    alive = false;
            

}
        }
    }
    else
    

{
        vely += 200.f * dt;
        x += velx * dt;
        y += vely * dt;

        animTimer += dt;
        if (animTimer >= 0.08f)
        

{
            animTimer = 0;
            animFrame = (animFrame + 1) % 6;
        }

        if (y > 900.f || x < -1000.f || x > 50000.f)
        

{
        

    alive = false;
        

}
    }
}

void M15ARocketProjectile::update(float dt, const class World& world)


{
    update(dt);
    if (!isExploding)
    

{
        int tileCol = (int)(x / 32.f);
        int tileRow = (int)(y / 32.f);
        if (world.isSolid(tileCol, tileRow) || y >= 1500.f)
        

{
        

    onHit();
        

}
    }
}

void M15ARocketProjectile::onHit()


{
    if (!isExploding)
    

{
        isExploding = true;
        explodeFrame = 0;
        explodeTimer = 0;
        velx = 0;
        vely = 0;
    }
}



void M15ARocketProjectile::render(sf::RenderWindow& window) 

{

    render(window, 0, 0);

}


void M15ARocketProjectile::render(sf::RenderWindow& window, float camX, float camY)


{
    if (!alive)
    {
        return;
    }

    if (isExploding)
    

{
        sf::IntRect rect = explodeFrames[explodeFrame % 5];
        sprite.setTextureRect(rect);
        sprite.setOrigin((float)rect.width / 2.f, (float)rect.height / 2.f);
        sprite.setPosition(x - camX, y - camY);
        sprite.setScale(2.5f, 2.5f);
        window.draw(sprite);
    }
    else
    

{
        sf::IntRect rect = rocketFrames[animFrame % 6];
        sprite.setTextureRect(rect);
        sprite.setOrigin((float)rect.width / 2.f, (float)rect.height / 2.f);
        sprite.setPosition(x - camX, y - camY);

        if (velx < 0)
            sprite.setScale(2.5f, 2.5f);
        else
            sprite.setScale(-2.5f, 2.5f);

        window.draw(sprite);
    }
}

sf::FloatRect M15ARocketProjectile::getBounds() const


{
    if (isExploding)
    

{
    

    return sf::FloatRect(0, 0, 0, 0);
    

}
    sf::IntRect rect = rocketFrames[animFrame % 6];
    return sf::FloatRect(x - (float)rect.width * 1.25f, y - (float)rect.height * 1.25f, (float)rect.width * 2.5f, (float)rect.height * 2.5f);
}







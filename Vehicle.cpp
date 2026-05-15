#include "Vehicle.h"
#include "Projectile.h"
#include "Enemy.h" // provides registerPlayerProjectile
#include <iostream>

#include "Soldier.h"
#include <iostream>

// BASE VEHICLE IMPLEMENTATIONS

void Vehicle::board(Soldier* s)


{


    pilot = s;


}
void Vehicle::eject()


{


    pilot = NULL;


}

// METAL SLUG TANK

const float MetalSlugTank::SCALE = 3; 

MetalSlugTank::MetalSlugTank(float startX, float startY)
    : GroundVehicle(startX, startY),
    currentState(IDLE),
    animTimer(0), // start anim time at 0
    currentFrame(0),
    facingRight(true)


{
    // Load all animation spritesheets
    tIdle.loadFromFile("Sprites/Vehicles/metalslug/idle.png");
    tDriving.loadFromFile("Sprites/Vehicles/metalslug/driving.png");
    tCDriving.loadFromFile("Sprites/Vehicles/metalslug/cdriving.png");
    tJump.loadFromFile("Sprites/Vehicles/metalslug/jump.png");
    tFire.loadFromFile("Sprites/Vehicles/metalslug/fire.png");
    tEnter.loadFromFile("Sprites/Vehicles/metalslug/enter.png");
    tExit.loadFromFile("Sprites/Vehicles/metalslug/exit.png");

    sprite.setScale(SCALE, SCALE);
    updateFrameSize();
}

void MetalSlugTank::setState(State s)


{
    if (currentState != s)
    

{
        currentState = s;
        currentFrame = 0;
        animTimer = 0;
        updateFrameSize();
    }
}

void MetalSlugTank::updateFrameSize()


{
    sf::IntRect frameRect(0, 0, 0, 0);

    switch (currentState)
    

{
    case IDLE:
        sprite.setTexture(tIdle);
        frameRect = sf::IntRect(0, 0, 60, 56);
        break;
    case DRIVING:
        sprite.setTexture(tDriving);
        frameRect = sf::IntRect(0, 0, 62, 56);
        break;
    case CROUCH_DRIVING:
        sprite.setTexture(tCDriving);
        frameRect = sf::IntRect(0, 0, 72, 56);
        break;
    case JUMPING:
        sprite.setTexture(tJump);
        frameRect = sf::IntRect(0, 0, 55, 71);
        break;
    case FIRING:
        sprite.setTexture(tFire);
        frameRect = sf::IntRect(0, 0, 60, 56);
        break;
    case ENTERING:
        sprite.setTexture(tEnter);
        frameRect = sf::IntRect(0, 0, 70, 46);
        break;
    case EXITING:
        sprite.setTexture(tExit);
        frameRect = sf::IntRect(0, 0, 70, 46);
        break;
    }

    sprite.setTextureRect(frameRect);
    sprite.setOrigin(frameRect.width / 2.f, (float)frameRect.height);
}


// Resolves user hardware control inputs for movement

void MetalSlugTank::handleInput(float dt)


{
    if (fireCooldownTimer > 0)
    

{
    

    fireCooldownTimer -= dt;
    

}
    if (!pilot)
    

{
        vehVelX = 0;
        setState(IDLE);
        return;
    }

    float speedVal = 280;
    bool moved = false;
    bool firing = sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    
{
        vehVelX = speedVal;
        facingRight = true;
        moved = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    
{
        vehVelX = -speedVal;
        facingRight = false;
        moved = true;
    }
    else
    

{
    

    vehVelX = 0;
    

}
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    
{
        if (vehOnGround)
        
{
            vehVelY = -650;
            vehOnGround = false;
        }
    }

    if (firing && fireCooldownTimer <= 0)
    

{
        float bX;
        if (facingRight)
        

{
        

    bX = x + 40;
        

}
        else
        

{
        

    bX = x - 40;
        

}
        float bY = y - 35;
        
        float bulletSpeed;
        if (facingRight)
        

{
        

    bulletSpeed = 900;
        

}
        else
        

{
        

    bulletSpeed = -900;
        

}
        Projectile* p = new StraightProjectile(bX, bY, bulletSpeed, 0, 4, true, sf::Color::Yellow, 12, 6);
        registerPlayerProjectile(p);
        fireCooldownTimer = 0.12f;
    }

    if (firing)
    

{
    

    setState(FIRING);
    

}
    else if (!vehOnGround)
    

{
    

    setState(JUMPING);
    

}
    else if (moved)
    

{
    

    setState(DRIVING);
    

}
    else
    

{
    

    setState(IDLE);
    

}
}




void MetalSlugTank::update(float dt) 

{
   
    if (!vehOnGround)
    

{
    

    vehVelY += 980 * dt;
    

}
    else if (vehVelY > 0)
    

{
    

    vehVelY = 0;
    

}
    x += vehVelX * dt;
    y += vehVelY * dt;

    float halfWidth = 90.f;
    if (x + halfWidth > 2080 && x - halfWidth < 4000)
    
{
        if (x < 3000)
        
{
            x = 2080 - halfWidth; // stay left
            vehVelX = 0;
        }
        else
        
{
            x = 4000 + halfWidth; // stay right
            vehVelX = 0;
        }
    }

    animTimer += dt;
    const float frameDur = 0.1f; // time for each frame

    int totalFrames = 1;
    switch (currentState)
    

{
    case IDLE: 
    {
        totalFrames = 3; 
        break;
    }
    case DRIVING: 
    {
        totalFrames = 20; 
        break;
    }
    case CROUCH_DRIVING: 
    {
        totalFrames = 20; 
        break;
    }
    case JUMPING: 
    {
        totalFrames = 4; 
        break;
    }
    case FIRING: 
    {
        totalFrames = 10; 
        break;
    }
    case ENTERING: 
    {
        totalFrames = 5; 
        break;
    }
    case EXITING: 
    {
        totalFrames = 5; 
        break;
    }
    }

    if (animTimer >= frameDur)
    

{
        animTimer = 0;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    // Calculate active frame rectangle
    sf::IntRect currentRect = sprite.getTextureRect();
    currentRect.left = currentFrame * currentRect.width;
    sprite.setTextureRect(currentRect);
}



void MetalSlugTank::render(sf::RenderWindow& w, float camX, float camY) 

{
    sprite.setPosition(x - camX, y - camY);
    
    if (facingRight)
    

{
    

    sprite.setScale(SCALE, SCALE);
    

}
    else
    

{
    

    sprite.setScale(-SCALE, SCALE);
    

}
    w.draw(sprite);
}

sf::FloatRect MetalSlugTank::getBounds() const 

{
    
    float w = 60.f * SCALE;
    float h = 56.f * SCALE;
    return sf::FloatRect(x - w / 2.f, y - h, w, h);
}


// SLUG FLYER

const float SlugFlyer::SCALE = 3;

SlugFlyer::SlugFlyer(float startX, float startY)
    : AerialVehicle(startX, startY),
    currentState(IDLE_DRIVING),
    animTimer(0), // init to 0
    currentFrame(0),
    facingRight(true)


{
    tIdleDriving.loadFromFile("Sprites/Vehicles/slugflyer/idle-driving.png");
    tEnter.loadFromFile("Sprites/Vehicles/slugflyer/enter-takeoff.png");
    tExit.loadFromFile("Sprites/Vehicles/slugflyer/land-exit.png");

    sprite.setScale(SCALE, SCALE);
    updateFrameSize();
}

void SlugFlyer::setState(State s)


{
    if (currentState != s)
    

{
        currentState = s;
        currentFrame = 0;
        animTimer = 0;
        updateFrameSize();
    }
}

void SlugFlyer::updateFrameSize()


{
    sf::IntRect frameRect(0, 0, 0, 0);

    switch (currentState)
    

{
    case IDLE_DRIVING:
        sprite.setTexture(tIdleDriving);
        frameRect = sf::IntRect(0, 0, 79, 52);
        break;
    case ENTER_TAKEOFF:
        sprite.setTexture(tEnter);
        frameRect = sf::IntRect(0, 0, 79, 64);
        break;
    case LAND_EXIT:
        sprite.setTexture(tExit);
        frameRect = sf::IntRect(0, 0, 79, 64);
        break;
    }

    sprite.setTextureRect(frameRect);
    sprite.setOrigin(frameRect.width / 2.f, (float)frameRect.height / 2.f); // Aerial, center origin
}



void SlugFlyer::handleInput(float dt) 

{
    // cooldown handle
    if (fireCooldownTimer > 0)
    

{
    

    fireCooldownTimer -= dt;
    

}
    if (!pilot)
    

{
        vehVelX = 0;
        vehVelY = 0;
        return;
    }

    float speedVal = 350;
    vehVelX = 0;
    vehVelY = 0; // Complete rigid flight lockdown (no automatic movement)

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    

{
        vehVelX = speedVal;
        facingRight = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    

{
        vehVelX = -speedVal;
        facingRight = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    
{
        vehVelY = -speedVal;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    
{
        vehVelY = speedVal;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && fireCooldownTimer <= 0)
    

{
        float bX;
        if (facingRight)
        

{
        

    bX = x + 40;
        

}
        else
        

{
        

    bX = x - 40;
        

}
        float bY = y + 10;
        
        float projVel;
        if (facingRight)
        

{
        

    projVel = 1000;
        

}
        else
        

{
        

    projVel = -1000;
        

}
        // make new projectile
        Projectile* p = new StraightProjectile(bX, bY, projVel, 0, 3, true, sf::Color(255, 200, 100), 10, 5);
        registerPlayerProjectile(p);
        
        // Drop bomb downward
        Projectile* bomb = new BombProjectile(x, y + 20, projVel * 0.1f, 250.f, 15, true);
        registerPlayerProjectile(bomb);

        fireCooldownTimer = 0.10f;
    }
}



void SlugFlyer::update(float dt) 

{
    x += vehVelX * dt;
    y += vehVelY * dt;

    // Rigid containment clamps to permanently prevent vehicle escaping the playable viewport
    if (y < 50.f) { y = 50.f; vehVelY = 0; } // Ceiling
    if (y > 850.f) { y = 850.f; vehVelY = 0; } // Safety Floor range preventing infinite falling

    animTimer += dt;
    const float frameDur = 0.1f;

    int totalFrames = 1;
    switch (currentState) 

{
    case IDLE_DRIVING: 
    {
        totalFrames = 5; 
        break;
    }
    case ENTER_TAKEOFF: 
    {
        totalFrames = 6; 
        break;
    }
    case LAND_EXIT: 
    {
        totalFrames = 6; 
        break;
    }
    }

    if (animTimer >= frameDur)
    

{
        animTimer = 0;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    sf::IntRect currentRect = sprite.getTextureRect();
    currentRect.left = currentFrame * currentRect.width;
    sprite.setTextureRect(currentRect);
}




void SlugFlyer::render(sf::RenderWindow& w, float camX, float camY)


{
    sprite.setPosition(x - camX, y - camY);
    
    if (facingRight)
    

{
    

    sprite.setScale(SCALE, SCALE);
    

}
    else
    

{
    

    sprite.setScale(-SCALE, SCALE);
    

}
    w.draw(sprite);

    if (pilot)
    {
        float oldX = pilot->getX();
        float oldY = pilot->getY();
        bool oldFacing = pilot->isFacingRight();

        pilot->renderOnlyUpper = true;
        pilot->setFacingRight(facingRight);

        float pOffsetX = facingRight ? 15.f : -15.f;
        pilot->setPosition(x + pOffsetX, y - 55.f); 
        
        pilot->render(w, camX, camY);
        
        pilot->setPosition(oldX, oldY);
        pilot->setFacingRight(oldFacing);
        pilot->renderOnlyUpper = false;
    }
}

sf::FloatRect SlugFlyer::getBounds() const 

{
    // Frame 79x52, center origin
    float w = 79.f * SCALE;
    float h = 52.f * SCALE;
    return sf::FloatRect(x - w / 2.f, y - h / 2.f, w, h);
}

// SLUG MARIner
const float SlugMariner::SCALE = 3;

SlugMariner::SlugMariner(float startX, float startY)
    : AquaticVehicle(startX, startY),
    currentState(DRIVING),
    animTimer(0),
    currentFrame(0),
    facingRight(true)


{
    tDriving.loadFromFile("Sprites/Vehicles/slugmariner/driving.png");
    tEnter.loadFromFile("Sprites/Vehicles/slugmariner/enter.png");
    tExit.loadFromFile("Sprites/Vehicles/slugmariner/exit.png");

    sprite.setScale(SCALE, SCALE);
    updateFrameSize();
}

void SlugMariner::setState(State s)


{
    if (currentState != s)
    

{
        currentState = s;
        currentFrame = 0;
        animTimer = 0;
        updateFrameSize();
    }
}

void SlugMariner::updateFrameSize()


{
    sf::IntRect frameRect(0, 0, 0, 0);

    switch (currentState)
    

{
    case DRIVING:
        sprite.setTexture(tDriving);
        frameRect = sf::IntRect(0, 0, 57, 51);
        break;
    case ENTERING:
        sprite.setTexture(tEnter);
        frameRect = sf::IntRect(0, 0, 56, 65);
        break;
    case EXITING:
        sprite.setTexture(tExit);
        frameRect = sf::IntRect(0, 0, 56, 65);
        break;
    }

    sprite.setTextureRect(frameRect);
    sprite.setOrigin(frameRect.width / 2.f, (float)frameRect.height / 2.f); // Underwater
}




void SlugMariner::handleInput(float dt) 

{
    // cooldown decreasing logic
    if (fireCooldownTimer > 0)
    

{
    

    fireCooldownTimer -= dt;
    

}
    if (!pilot)
    

{
        vehVelX = 0;
        vehVelY = 0;
        return;
    }

    float speedVal = 250;
    vehVelX = 0;
    vehVelY = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    

{
        vehVelX = speedVal;
        facingRight = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    

{
        vehVelX = -speedVal;
        facingRight = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    

{
    

    vehVelY = -speedVal;
    

}
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    

{
    

    vehVelY = speedVal;
    

}
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && fireCooldownTimer <= 0)
    

{
        float bX;
        if (facingRight)
        

{
        

    bX = x + 35;
        

}
        else
        

{
        

    bX = x - 35;
        

}
        float bY = y + 15;
        
        float tVel;
        if (facingRight)
        

{
        

    tVel = 600;
        

}
        else
        

{
        

    tVel = -600;
        

}
        // adding new straight projectile
        Projectile* p = new StraightProjectile(bX, bY, tVel, 0, 6, true, sf::Color::Cyan, 20, 8);
        registerPlayerProjectile(p);
        fireCooldownTimer = 0.35f;
    }
}


void SlugMariner::update(float dt) 

{
    x += vehVelX * dt;
    y += vehVelY * dt;

    if (x < 2100)
    

{
    

    x = 2100;
    

}
    if (x > 4000)
    

{
    

    x = 4000;
    

}
    if (y < 580)
    

{
    

    y = 580; // sea surface limit
    

}
    animTimer += dt;
    const float frameDur = 0.12f;

    int totalFrames = 1;
    switch (currentState)
    

{
    case DRIVING: 
    {
        totalFrames = 4; 
        break;
    }
    case ENTERING: 
    {
        totalFrames = 6; 
        break;
    }
    case EXITING: 
    {
        totalFrames = 6; 
        break;
    }
    }

    if (animTimer >= frameDur)
    

{
        animTimer = 0;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    sf::IntRect currentRect = sprite.getTextureRect();
    currentRect.left = currentFrame * currentRect.width;
    sprite.setTextureRect(currentRect);
}



void SlugMariner::render(sf::RenderWindow& w, float camX, float camY)


{
    sprite.setPosition(x - camX, y - camY);
  
    if (facingRight)
    

{

    sprite.setScale(SCALE, SCALE);
    

}
    else
    

{   

    sprite.setScale(-SCALE, SCALE);
    

}
    w.draw(sprite);
}

sf::FloatRect SlugMariner::getBounds() const 

{
   
    float w = 57.f * SCALE;
    float h = 51.f * SCALE;
    return sf::FloatRect(x - w / 2.f, y - h / 2.f, w, h);
}

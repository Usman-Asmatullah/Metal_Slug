
#pragma once
#include "libraries.h"

// Camera

class Camera {
public:
    float x, y;           // top-left world pixel of the viewport
    static const int W = 1600;
    static const int H = 900;

    Camera() : x(0), y(0), tx(0), ty(0) 

{

}
    void follow(float playerWorldX, float playerWorldY) 

{
        tx = playerWorldX - W * 0.30f;   // player at 30% from left
        ty = playerWorldY - H * 0.55f;
        if (ty < -200.f)
        {
            ty = -200.f;
        }
    }


    // Advances state calculations by frame time increment

    void update(float dt) 

{
    float k = dt * 10.f;
    if (k > 1.f) k = 1.f; // linear lerp capped
        x += (tx - x) * k;
        y += (ty - y) * k;
    }

    void snap() 

{

    x = tx;
    y = ty;

}
private:
    float tx, ty;
};





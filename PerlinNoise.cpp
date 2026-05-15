#include "PerlinNoise.h"
#include <cmath>

#include <cstdlib>


PerlinNoise::PerlinNoise(int seed) 

{
    for (int i = 0; i < 256; i++)
    {
        perm[i] = i;
    }
    srand(seed);
    for (int i = 255; i > 0; i--) 

{
        int j = rand() % (i + 1);
    int tmp = perm[i];
    perm[i] = perm[j];
    perm[j] = tmp;
    }
    for (int i = 0; i < 256; i++)
    {
        perm[256 + i] = perm[i];
    }
}

float PerlinNoise::fade(float t) 

{

    return t * t * t * (t * (t * 6 - 15) + 10);

}
float PerlinNoise::lerp(float t, float a, float b) 

{

    return a + t * (b - a);

}
float PerlinNoise::grad(int hash, float x, float y) 

{
    int h = hash & 7;
    float u;
    if (h < 4)
    
{
    
    u = x;
    
}
    else
    
{
    
    u = y;
    
}
    float v;
    if (h < 4)
    
{
    
    v = y;
    
}
    else
    
{
    
    v = x;
    
}
    float val1;
    if ((h & 1) != 0)
    
{
    
    val1 = -u;
    
}
    else
    
{
    
    val1 = u;
    
}
    float val2;
    if ((h & 2) != 0)
    
{
    
    val2 = -v;
    
}
    else
    
{
    
    val2 = v;
    
}
    return val1 + val2;
}

float PerlinNoise::noise(float x, float y) 

{
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    x -= floor(x);
    y -= floor(y);
    float u = fade(x), v = fade(y);
    int A = perm[X] + Y, B = perm[X + 1] + Y;
    return lerp(v,
        lerp(u, grad(perm[A], x, y), grad(perm[B], x - 1, y)),
        lerp(u, grad(perm[A + 1], x, y - 1), grad(perm[B + 1], x - 1, y - 1)));
}


FractalNoise::FractalNoise(int seed, int oct, float pers)
    : pn(seed), octaves(oct), persistence(pers) 

{

}
float FractalNoise::sample(float x, float y) 

{
    float val = 0, amp = 1, freq = 1, maxVal = 0;
    for (int i = 0; i < octaves; i++) 

{
        val += pn.noise(x * freq, y * freq) * amp;
        maxVal += amp;
        amp *= persistence;
        freq *= 2;
    }
    return val / maxVal;
}







#pragma once

// Perlin Noise

class PerlinNoise {
private:
    int perm[512];      // permutation table
    float fade(float t);
    float lerp(float t, float a, float b);
    float grad(int hash, float x, float y);
public:
    PerlinNoise(int seed = 0);
    float noise(float x, float y);   
};

// Fractal Noise

class FractalNoise {
private:
    PerlinNoise pn;
    int octaves;
    float persistence;
public:
    FractalNoise(int seed, int octaves = 6, float persistence = 0.5f);
    float sample(float x, float y); 
};

// Terrain Profile

class TerrainProfile {
public:
    virtual float getAmplitude() const = 0;
    virtual float getOceanDepth() const = 0;
    virtual int getOctaves() const = 0;
    virtual ~TerrainProfile() 

{

}
};

// Amplified Profile

class AmplifiedProfile : public TerrainProfile {
public:
    float getAmplitude() const 

{

    return 2.0f;

}
    float getOceanDepth() const 

{

    return 2.0f;

}
    int getOctaves() const 

{

    return 8;

}
};

// Flat Profile

class FlatProfile : public TerrainProfile {
public:
    float getAmplitude() const 

{

    return 0.3f;

}
    float getOceanDepth() const 

{

    return 0.3f;

}
    int getOctaves() const 

{

    return 3;

}
};

// Normal Profile

class NormalProfile : public TerrainProfile {
public:
    float getAmplitude() const 

{

    return 1.0f;

}
    float getOceanDepth() const 

{

    return 1.0f;

}
    int getOctaves() const 

{

    return 6;

}
};

// Terrain Profile Factory

class TerrainProfileFactory {
public:
    static TerrainProfile* create(int type) 

{
        if (type == 0)
        {
            return new AmplifiedProfile();
        }
        if (type == 1)
        {
            return new FlatProfile();
        }
        return new NormalProfile();
    }
};






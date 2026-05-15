#pragma once
#include "libraries.h"
#include "PerlinNoise.h"

typedef unsigned char BlockType;
static const BlockType AIR = 0;
static const BlockType GRASS = 1;
static const BlockType DIRT = 2;
static const BlockType STONE = 3;
static const BlockType BEDROCK = 4;
static const BlockType ONE_WAY = 5;
static const BlockType DESTRUCTIBLE = 6;

static const int CELL = 32;
static const int WORLD_H = 40;
static const int CHUNK_W = 32;

struct Chunk {
    int chunkX;
    BlockType blocks[CHUNK_W][WORLD_H];
    Chunk() : chunkX(0) 
    {
        for (int x = 0; x < CHUNK_W; x++)
            for (int y = 0; y < WORLD_H; y++)
                blocks[x][y] = AIR;
    }
};

struct ChunkEntry {
    int key;
    Chunk* chunk;
    ChunkEntry() : key(0), chunk(0) 
    {
    }
};

// Chunk Map

class ChunkMap {
public:
    ChunkMap() : entries(0), count(0), capacity(0) 
    {
    }
    ~ChunkMap() 
    {
        for (int i = 0; i < count; i++)
        {
            delete entries[i].chunk;
        }
        delete[] entries;
    }
    Chunk* find(int key) const 
    {
        for (int i = 0; i < count; i++)
            if (entries[i].key == key)
            {
                return entries[i].chunk;
            }
        return 0;
    }
    void insert(int key, Chunk* ch) 
    {
        if (count >= capacity)
        {
            grow();
        }
        entries[count].key = key;
        entries[count].chunk = ch;
        count++;
    }
    int getCount() const 
    {
        return count;
    }
    const ChunkEntry& getEntry(int i) const 
    {
        return entries[i];
    }
private:
    ChunkEntry* entries;
    int count, capacity;
    void grow() 
    {
        int newCap;
        if (capacity == 0)
        {
            newCap = 16;
        }
        else
        {
            newCap = capacity * 2;
        }
        ChunkEntry* neo = new ChunkEntry[newCap];
        for (int i = 0; i < count; i++)
        {
            neo[i] = entries[i];
        }
        delete[] entries;
        entries = neo;
        capacity = newCap;
    }
};

struct BlockTextures {
    sf::Texture* grass;
    sf::Texture* dirt;
    sf::Texture* stone;
    sf::Texture* bedrock;
    sf::Texture* oneWay;
};

// World

class World {
public:
    World(int seed = 42, int profileType = 2) : noise(seed, 6, 0.5f) 
    {
        profile = TerrainProfileFactory::create(profileType);
        for (int cx = -1; cx <= 8; cx++)
        {
            getOrGen(cx);
        }
    }
    ~World() 
    {
        delete profile;
    }
    BlockType getBlock(int bx, int by) const 
    {
        if (by < 0 || by >= WORLD_H)
        {
            return AIR;
        }
        int cx, lx;
        worldToChunk(bx, cx, lx);
        Chunk* ch = chunks.find(cx);
        if (ch == 0)
        {
            return AIR;
        }
        return ch->blocks[lx][by];
    }

    void destroyBlock(int bx, int by) 
    {
        if (by < 0 || by >= WORLD_H)
        {
            return;
        }
        int cx, lx;
        worldToChunk(bx, cx, lx);
        Chunk* ch = chunks.find(cx);
        if (ch != 0) 
        {
            if (ch->blocks[lx][by] == DESTRUCTIBLE) 
            {
                ch->blocks[lx][by] = AIR;
            }
        }
    }

    bool isSolid(int bx, int by) const 
    {
        BlockType b = getBlock(bx, by);
        if (b == DESTRUCTIBLE) 
        {
            const_cast<World*>(this)->destroyBlock(bx, by);
            return true;
        }
        return b == GRASS || b == DIRT || b == STONE || b == BEDROCK;
    }
    bool isOneWay(int bx, int by) const 
    {
        return getBlock(bx, by) == ONE_WAY;
    }
    void update(float camPx, float dt) 
    {
        m_timer += dt;
        if (m_timer >= 15.f) 
        {
            m_timer = 0;
            // m_biomeShift += 20; // Keep biomes stationary to prevent water disappearing
        }
        int cCX = (int)(camPx / (CHUNK_W * CELL));
        for (int cx = cCX - 2; cx <= cCX + 7; cx++)
        {
            getOrGen(cx);
        }
    }

    int surfaceAt(int bx) const 
    {
        int cx, lx;
        worldToChunk(bx, cx, lx);
        Chunk* ch = chunks.find(cx);
        if (ch == 0)
        {
            return WORLD_H / 2;
        }
        for (int by = 0; by < WORLD_H; by++)
            if (ch->blocks[lx][by] == GRASS)
            {
                return by;
            }
        return WORLD_H / 2;
    }

    int getBiomeAt(int bx) const 
    {
      
        if (GameData::campaignMode) 
        {
            long long region = (bx >= 0) ? (bx / 100) : ((bx - 99) / 100);
            unsigned long long seed = (unsigned long long)abs((int)region) * 4391 + 76543;
            return (int)((seed ^ (seed >> 3)) % 3);
        }
        
        int shiftedBX = bx + m_biomeShift;
        if (shiftedBX >= 65 && shiftedBX < 125) return 1; // Aquatic
        if (shiftedBX >= 145) return 2;                   // Aerial
        return 0;                                         // Plains
    }

    float getBiomeBaseElevation(int bio, int bx) const 
    {
        float b = (float)(WORLD_H * 0.45f);
        if (bio == 1) 
        {
            b += 5.0f; 
        }
        else if (bio == 2) 
        {
            int segment = abs(bx) / 20;
            b -= (segment % 2 == 0) ? 12.0f : 8.0f;
            if ((abs(bx) / 5) % 3 == 0) b -= 2.0f;
        }
        return b;
    }

    int getBiomeType(float x) const 
    {
        return getBiomeAt((int)(x / CELL));
    }


    void render(sf::RenderWindow& window,
        sf::Sprite& sprite,
        const BlockTextures& tex,
        float camX, float camY,
        int screenW, int screenH) const
    {
        int startCX = (int)(camX / (CHUNK_W * CELL)) - 1;
        int endCX = startCX + (screenW / (CHUNK_W * CELL)) + 3;

        for (int cx = startCX; cx <= endCX; cx++) 
        {
            Chunk* ch = chunks.find(cx);
            if (ch == 0) continue;

            for (int lx = 0; lx < CHUNK_W; lx++) 
            {
                int bx = cx * CHUNK_W + lx;
                float px = bx * CELL - camX;
                if (px > screenW + CELL || px < -CELL) continue;

                for (int by = 0; by < WORLD_H; by++) 
                {
                    BlockType bt = ch->blocks[lx][by];
                    float py = by * CELL - camY;
                    if (py > screenH + CELL || py < -CELL) continue;

                    sf::Texture* t = 0;
                    bool tintDestructible = false;
                    bool isWater = false;

                    if (bt == AIR) 
                    {
                        if (getBiomeAt(bx) == 1 && by >= (int)(WORLD_H * 0.45f)) 
                        {
                            t = tex.stone;
                            isWater = true;
                        }
                        else 
                        {
                            continue;
                        }
                    }
                    else if (bt == GRASS) t = tex.grass;
                    else if (bt == DIRT) t = tex.dirt;
                    else if (bt == STONE) t = tex.stone;
                    else if (bt == BEDROCK) t = tex.bedrock;
                    else if (bt == ONE_WAY) t = tex.oneWay;
                    else if (bt == DESTRUCTIBLE) 
                    {
                        if (by % 2 == 0) t = tex.oneWay;
                        else t = tex.bedrock;
                        tintDestructible = true;
                    }
                    if (t == 0) continue;

                    sprite.setTexture(*t);
                    if (isWater) 
                    {
                        sprite.setColor(sf::Color(0, 150, 255, 120)); // Color: Semi-Transparent Ocean Blue 
                    }
                    else if (tintDestructible) 
                    {
                        sprite.setColor(sf::Color(255, 100, 100)); // Color: Damaged Light Red 
                    }
                    else 
                    {
                        sprite.setColor(sf::Color::White); // Color: White
                    }
                    sprite.setScale((float)CELL / t->getSize().x, (float)CELL / t->getSize().y);
                    sprite.setPosition(px, py);
                    window.draw(sprite);
                }
            }
        }
    }

private:
    ChunkMap chunks;
    FractalNoise noise;
    TerrainProfile* profile;
    float m_timer = 0;
    int m_biomeShift = 0;

    static void worldToChunk(int bx, int& cx, int& lx) 
    {
        if (bx >= 0) 
        {
            cx = bx / CHUNK_W;
            lx = bx % CHUNK_W;
        }
        else 
        {
            cx = (bx - CHUNK_W + 1) / CHUNK_W;
            lx = bx - cx * CHUNK_W;
        }
    }
    static int clamp(int v, int lo, int hi) 
    {
        if (v < lo) return lo;
        if (v > hi) return hi;
        return v;
    }
    Chunk* getOrGen(int cx) 
    {
        Chunk* e = chunks.find(cx);
        if (e != 0) return e;
        Chunk* ch = generate(cx);
        chunks.insert(cx, ch);
        return ch;
    }
    Chunk* generate(int cx) 
    {
        Chunk* ch = new Chunk();
        ch->chunkX = cx;
        float amp = profile->getAmplitude();
        int surf[CHUNK_W];
        
        for (int lx = 0; lx < CHUNK_W; lx++) 
        {
            int bx = cx * CHUNK_W + lx;
            float nx = bx * 0.035f;
            float h = noise.sample(nx, 0.5f) * amp;
            
            int currentBio = getBiomeAt(bx);
            float targetBase = getBiomeBaseElevation(currentBio, bx);

           
        if (GameData::campaignMode) 
            {
                int region = (bx >= 0) ? (bx / 100) : ((bx - 99) / 100);
                int localX = bx - (region * 100);
                if (localX >= 80) 
                {
                    int nextB = getBiomeAt((region + 1) * 100);
                    float nextTarget = getBiomeBaseElevation(nextB, (region + 1) * 100);
                    float t = (float)(localX - 80) / 20.0f; 
                    targetBase = targetBase * (1.0f - t) + nextTarget * t;
                }
            }
            else 
            {
                int sBX = bx + m_biomeShift;
                if (sBX >= 125 && sBX < 145) 
                {
                    targetBase = (float)(WORLD_H * 0.45f) + 5.0f - (float)(sBX - 125);
                }
            }

            int computedIntBase = (int)targetBase;
            surf[lx] = clamp(computedIntBase + (int)(h * WORLD_H * 0.17f), 3, WORLD_H - 5);
        }

        for (int lx = 0; lx < CHUNK_W; lx++) 
        {
            int s = surf[lx];
            int gbx = cx * CHUNK_W + lx;
            int blockBiome = getBiomeAt(gbx);

            for (int by = 0; by < WORLD_H; by++) 
            {
                if (by == WORLD_H - 1) 
                {
                    ch->blocks[lx][by] = BEDROCK;
                }
                else if (blockBiome == 1) 
                {
                    if (by >= s) ch->blocks[lx][by] = STONE;
                }
                else 
                {
                    if (by > s + 6) ch->blocks[lx][by] = STONE;
                    else if (by > s + 1) ch->blocks[lx][by] = DIRT;
                    else if (by == s) ch->blocks[lx][by] = GRASS;
                    else if (by > s) ch->blocks[lx][by] = DIRT;
                }
            }

            if ((abs(gbx) % 15) == 8 && s > 5) 
            {
                ch->blocks[lx][s - 1] = DESTRUCTIBLE;
                ch->blocks[lx][s - 2] = DESTRUCTIBLE;
                ch->blocks[lx][s - 3] = DESTRUCTIBLE;
            }

            if ((abs(gbx) % 18) == 12 && s > 12) 
            {
                int airY = s - 6;
                if (airY >= 5 && airY < WORLD_H) 
                {
                    ch->blocks[lx][airY] = DESTRUCTIBLE;
                    ch->blocks[lx][airY - 1] = DESTRUCTIBLE;
                    ch->blocks[lx][airY - 2] = DESTRUCTIBLE;
                }
            }
            
            if (blockBiome == 2) 
            {
                int pattern = (abs(gbx) / 10) % 3;
                if (pattern == 0) 
                {
                    int stairStep = (abs(gbx) % 15);
                    int platformY = s - 3 - stairStep;
                    if (stairStep >= 2 && stairStep <= 8 && platformY >= 0 && platformY < WORLD_H) 
                    {
                        ch->blocks[lx][platformY] = ONE_WAY;
                    }
                } 
                else if (pattern == 1) 
                {
                    if ((abs(gbx) % 6) < 3) 
                    {
                        if (s - 4 >= 0 && s - 4 < WORLD_H) ch->blocks[lx][s - 4] = ONE_WAY;
                        if (s - 8 >= 0 && s - 8 < WORLD_H) ch->blocks[lx][s - 8] = ONE_WAY;
                    }
                } 
                else 
                {
                    if ((abs(gbx) % 4) == 0) 
                    {
                        if (s - 12 >= 0 && s - 12 < WORLD_H) ch->blocks[lx][s - 12] = ONE_WAY;
                    }
                }
            }
            else if (blockBiome == 0) 
            {
                if ((abs(gbx) % 10) == 4 && s > 9) 
                {
                    if (s - 5 >= 0 && s - 5 < WORLD_H) ch->blocks[lx][s - 5] = ONE_WAY;
                }
            }
        }
        return ch;
    }
};

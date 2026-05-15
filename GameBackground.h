#pragma once
#include "libraries.h"
#include <SFML/Graphics.hpp>

// Game Background

class GameBackground
{
public:
    GameBackground() : loaded(false), currentLevel(0) 

{

}
    // level = 1/2/3 for gameplay, 0 for intro/menu
    bool load(int level)
    

{
        currentLevel = level;
        loaded = false;

        const char* paths[4] = {
            "Sprites/Backgrounds/bg_intro.png",
            "Sprites/Backgrounds/bg_level1.png",
            "Sprites/Backgrounds/bg_level2.png",
            "Sprites/Backgrounds/bg_level3.png"
        };

        int idx = level;   // 0=intro, 1/2/3 = levels
        if (idx < 0)
        {
            idx = 0;
        }
        if (idx > 3)
        {
            idx = 3;
        }

        if (!texture.loadFromFile(paths[idx]) )
        {
            return false;
        }
        texture.setRepeated(false);
        sprite.setTexture(texture);
        loaded = true;
        return true;
    }


    void render(sf::RenderWindow& window, float camX,
        int screenW, int screenH) const
    

{
        if (!loaded)
        {
            return;
        }

        MyVector2u ts = texture.getSize();
        float scaleX = (float)screenW / (float)ts.x;
        float scaleY = (float)screenH / (float)ts.y;

        float bgOffX = camX * 0.20f;

        float maxShift = (float)ts.x - (float)screenW / scaleX;
        if (maxShift < 0)
        {
            maxShift = 0;
        }
        if (bgOffX > maxShift)
        {
            bgOffX = maxShift;
        }
        if (bgOffX < 0)
        {
            bgOffX = 0;
        }

        sprite.setTextureRect(sf::IntRect(
            (int)bgOffX, 0, ts.x, ts.y));
        sprite.setScale(scaleX, scaleY);
        sprite.setPosition(0, 0);
        window.draw(sprite);
    }

    bool isLoaded() const 

{

    return loaded;

}
    int getLevel() const 

{

    return currentLevel;

}
private:
    sf::Texture texture;
    mutable sf::Sprite sprite;
    bool loaded;
    int currentLevel;
};







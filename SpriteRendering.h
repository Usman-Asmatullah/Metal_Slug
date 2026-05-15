#pragma once
#include "libraries.h"
#include "World.h"   // for BlockTextures struct

// Sprite Renderer

class SpriteRenderer {
public:
    sf::Texture texGrass, texDirt, texStone, texBedrock, texOneWay;
    sf::Texture texPlayer, texHill, texWhite;
    sf::Texture texRedLight, texRedHeavy;

    SpriteRenderer() 

{
        if (!texGrass.loadFromFile("Sprites/blocks/grass_block_side.png")) 

{

    make(texGrass, 72, 160, 72);

}
        if (!texDirt.loadFromFile("Sprites/blocks/dirt.png")) 

{

    make(texDirt, 139, 90, 43);

}
        if (!texStone.loadFromFile("Sprites/blocks/stone.png")) 

{

    make(texStone, 120, 120, 120);

}
        if (!texBedrock.loadFromFile("Sprites/blocks/deepslate_top.png")) 

{

    make(texBedrock, 40, 40, 40);

}
        if (!texOneWay.loadFromFile("Sprites/blocks/andesite.png")) 

{

    make(texOneWay, 180, 140, 60);

}
        make(texPlayer, 50, 150, 220);
        if (!texHill.loadFromFile("Sprites/blocks/Map1.png"))
        

{
        

    make(texHill, 55, 105, 55);
        

}
        make(texWhite, 255, 255, 255);
        make(texRedLight, 255, 0, 0, 60);
        make(texRedHeavy, 255, 0, 0, 110);
    }

    BlockTextures getBlockTextures() 

{
        BlockTextures bt;
        bt.grass = &texGrass;
        bt.dirt = &texDirt;
        bt.stone = &texStone;
        bt.bedrock = &texBedrock;
        bt.oneWay = &texOneWay;
        return bt;
    }

    sf::Sprite sprite;

private:
    void make(sf::Texture& tex, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) 

{
        sf::Image img;
        img.create(1, 1, sf::Color(r, g, b, a));
        tex.loadFromImage(img);
    }
};





#pragma once
#include "libraries.h"
#include <SFML/Graphics.hpp>

// Intro Sequence

class IntroSequence
{
public:
    IntroSequence()
        : phase(0), timer(0), done(false), loaded(false),
        panX(0), titleScale(4.f),
        blinkTimer(0), blinkOn(true),
        pBlackTex(NULL)
    

{
    

}
    // bgPath   = "Sprites/Backgrounds/bg_intro.png"
    // blackTex = sr.texBlack  (1x1 black pixel texture for fades)
    bool load(const char* bgPath, const sf::Texture& blackTex)
    

{
        loaded = false;
        if (!bgTex.loadFromFile(bgPath) )
        {
            return false;
        }
        bgTex.setRepeated(false);
        bgSprite.setTexture(bgTex);
        pBlackTex = &blackTex;
        fadeSprite.setTexture(blackTex);
        reset();
        loaded = true;
        return true;
    }

    void reset()
    

{
        phase = 0;
        timer = 0;
        done = false;
        panX = 0;
        titleScale = 4.f;
        blinkTimer = 0;
        blinkOn = true;
    }

    void handleEvent(const sf::Event& ev)
    

{
        if (ev.type == sf::Event::KeyPressed ||
            ev.type == sf::Event::MouseButtonPressed)
            done = true;
    }


    // Advances state calculations by frame time increment

    void update(float dt)
    

{
        if (done || !loaded)
        {
            return;
        }

        blinkTimer += dt;
        if (blinkTimer >= 0.48f) 

{

    blinkTimer = 0;
    blinkOn = !blinkOn;

}
        timer += dt;

        switch (phase)
        

{
        case 0:                          // fade in
            if (timer >= 1.5f) 

{

    phase = 1;
    timer = 0;

}
            break;

        case 1:                          // pan right
            panX = (timer / 3.0f) * PAN_DIST;
            if (panX > PAN_DIST)
            {
                panX = PAN_DIST;
            }
            if (timer >= 3.0f) 

{

    phase = 2;
    timer = 0;
    titleScale = 4.f;

}
            break;

        case 2:                          // title slam
            titleScale = 4.f - (timer / 0.9f) * 3.f;
            if (titleScale < 1.f)
            {
                titleScale = 1.f;
            }
            if (timer >= 1.0f) 

{

    phase = 3;
    timer = 0;

}
            break;

        case 3:                          // blink prompt
            if (timer >= 2.5f) 

{

    phase = 4;
    timer = 0;

}
            break;

        case 4:                          // fade out
            if (timer >= 0.8f)
            {
                done = true;
            }
            break;
        }
    }


    // Draws current frame buffer pixels to visual display

    void render(sf::RenderWindow& window, sf::Font& font,
        int screenW, int screenH) const
    

{
        if (!loaded)
        {
            return;
        }

        MyVector2u ts = bgTex.getSize();
        float scaleX = (float)screenW / (float)ts.x;
        float scaleY = (float)screenH / (float)ts.y;

        int texPanX = (int)(panX / scaleX);

        bgSprite.setTextureRect(sf::IntRect(texPanX, 0, (int)ts.x, (int)ts.y));
        bgSprite.setScale(scaleX, scaleY);
        bgSprite.setPosition(0, 0);
        window.draw(bgSprite);

        {
            unsigned char alpha = calcFadeAlpha();
            if (alpha > 0 && pBlackTex != NULL)
            

{
                fadeSprite.setTexture(*pBlackTex);
                fadeSprite.setTextureRect(sf::IntRect(0, 0, 1, 1));
                fadeSprite.setScale((float)screenW, (float)screenH);
                fadeSprite.setPosition(0, 0);
                if (alpha >= 240)
                    window.draw(fadeSprite);
            }
        }

        if (phase >= 2)
        

{
            // Shadow
            sf::Text shadow;
            shadow.setFont(font);
            shadow.setString("METAL SLUG");
            shadow.setCharacterSize(88);
            shadow.setFillColor(sf::Color(20, 10, 0));

            float estWidth = 10 * 88.f * 0.45f;
            float estHeight = 88.f;
            float cx = (float)(screenW / 2) - (estWidth * titleScale) / 2.f;
            float cy = (float)(screenH / 2) - (estHeight * titleScale) / 2.f - 100.f;

            shadow.setScale(titleScale, titleScale);
            shadow.setPosition(cx + 5.f * titleScale, cy + 5.f * titleScale);
            window.draw(shadow);

            // Gold title
            sf::Text title;
            title.setFont(font);
            title.setString("METAL SLUG");
            title.setCharacterSize(88);
            title.setFillColor(sf::Color(255, 210, 0));
            title.setScale(titleScale, titleScale);
            title.setPosition(cx, cy);
            window.draw(title);

            // Tagline
            sf::Text tag;
            tag.setFont(font);
            tag.setString("SUPER VEHICLE - 001");
            tag.setCharacterSize(26);
            tag.setFillColor(sf::Color(210, 230, 255));
            float estTagWidth = 19 * 26.f * 0.45f;
            tag.setPosition((float)(screenW / 2) - estTagWidth / 2.f,
                cy + 92.f * titleScale + 8.f);
            window.draw(tag);
        }

        if (phase == 3 && blinkOn)
        

{
            sf::Text prompt;
            prompt.setFont(font);
            prompt.setString("- - -  PRESS  ENTER  TO  START  - - -");
            prompt.setCharacterSize(28);
            prompt.setFillColor(sf::Color(255, 255, 200));
            float estPromptWidth = 37 * 28.f * 0.45f;
            prompt.setPosition((float)(screenW / 2) - estPromptWidth / 2.f,
                (float)(screenH) * 0.73f);
            window.draw(prompt);
        }

        if (phase <= 1)
        

{
            sf::Text skip;
            skip.setFont(font);
            skip.setString("Press any key to skip");
            skip.setCharacterSize(15);
            skip.setFillColor(sf::Color(180, 180, 180));
            skip.setPosition(8.f, (float)(screenH - 22));
            window.draw(skip);
        }
    }

    bool isDone() const 

{

    return done;

}
    bool isLoaded() const 

{

    return loaded;

}
private:
    static const float PAN_DIST;   // screen pixels to pan

    sf::Texture bgTex;
    mutable sf::Sprite bgSprite;
    mutable sf::Sprite fadeSprite;
    const sf::Texture* pBlackTex;

    int phase;
    float timer;
    float panX;
    float titleScale;
    float blinkTimer;
    bool blinkOn;
    bool done;
    bool loaded;

    unsigned char calcFadeAlpha() const
    

{
        if (phase == 0)
        

{
            float t = 1.f - (timer / 1.5f);
            if (t < 0)
            {
                t = 0;
            }
            return (unsigned char)(t * 255.f);
        }
        if (phase == 4)
        

{
            float t = timer / 0.8f;
            if (t > 1.f)
            {
                t = 1.f;
            }
            return (unsigned char)(t * 255.f);
        }
        return 0;
    }
};

const float IntroSequence::PAN_DIST = 180.f;







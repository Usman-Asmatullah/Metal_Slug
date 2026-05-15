#pragma once
#include <SFML/Graphics.hpp>
#include "CustomBounds.h"
#include "ScoreManager.h"

// Main Menu State

class MainMenuState {
public:
    static const int MAIN = 0;
    static const int CREDITS = 1;
    static const int INSTRUCTIONS = 2;
    static const int SCOREBOARD = 3;

private:
    sf::Texture texStart, texOptions, texCredits, texExit, fallbackTexture;
    sf::Sprite backgroundSprite;
    sf::Font menuFont;
    sf::Text menuOptions[8];
    int selectedIndex;
    sf::RenderWindow* window;
    int subState;

    void initText(int index, const char* str, float yPos)
    {
        menuOptions[index].setFont(menuFont);
        menuOptions[index].setString(str);
        menuOptions[index].setCharacterSize(40);
        menuOptions[index].setFillColor(sf::Color::White); // Color: White
        menuOptions[index].setOutlineColor(sf::Color::Black); // Color: Black
        menuOptions[index].setOutlineThickness(2.0f);

        sf::FloatRect bounds = menuOptions[index].getGlobalBounds();
        menuOptions[index].setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        menuOptions[index].setPosition(1600.f / 2.0f, yPos);
    }

public:
    MainMenuState(void* gsm, sf::RenderWindow* win)
        : selectedIndex(0), window(win), subState(MAIN)
    {
        texStart.loadFromFile("Sprites/mainmenu_start.png");
        texOptions.loadFromFile("Sprites/mainmenu_options.png");
        texCredits.loadFromFile("Sprites/mainmenu_credits.png");
        texExit.loadFromFile("Sprites/mainmenu_exit.png");
        fallbackTexture.loadFromFile("Sprites/mainmenu.png");

        if (texStart.getSize().x > 0)
        {
            backgroundSprite.setTexture(texStart);
            backgroundSprite.setScale(1600.0f / texStart.getSize().x, 900.0f / texStart.getSize().y);
        }
        else if (fallbackTexture.getSize().x > 0)
        {
            backgroundSprite.setTexture(fallbackTexture);
            backgroundSprite.setScale(1600.0f / fallbackTexture.getSize().x, 900.0f / fallbackTexture.getSize().y);
        }

        menuFont.loadFromFile("C:/Windows/Fonts/arial.ttf");

        float startY = 180.0f;
        float spacing = 60.0f;

        initText(0, "1. Level 1", startY);
        initText(1, "2. Level 2", startY + spacing);
        initText(2, "3. Level 3", startY + spacing * 2);
        initText(3, "4. Campaign Mode", startY + spacing * 3);
        initText(4, "5. Credits", startY + spacing * 4);
        initText(5, "6. Instructions", startY + spacing * 5);
        initText(6, "7. Scoreboard", startY + spacing * 6);
        initText(7, "8. Exit", startY + spacing * 7);

        menuOptions[0].setFillColor(sf::Color::Yellow); // Color: Yellow
    }

    bool handleEvent(sf::Event& ev)
    {
        if (ev.type == sf::Event::KeyPressed)
        {
            if (subState != MAIN)
            {
                if (ev.key.code == sf::Keyboard::Enter || ev.key.code == sf::Keyboard::Escape)
                {
                    subState = MAIN;
                }
                return false;
            }

            if (ev.key.code == sf::Keyboard::Up)
            {
                menuOptions[selectedIndex].setFillColor(sf::Color::White); // Color: White
                selectedIndex = (selectedIndex - 1 + 8) % 8;
                menuOptions[selectedIndex].setFillColor(sf::Color::Yellow); // Color: Yellow
            }
            if (ev.key.code == sf::Keyboard::Down)
            {
                menuOptions[selectedIndex].setFillColor(sf::Color::White); // Color: White
                selectedIndex = (selectedIndex + 1) % 8;
                menuOptions[selectedIndex].setFillColor(sf::Color::Yellow); // Color: Yellow
            }
            if (ev.key.code == sf::Keyboard::Enter)
            {
                if (selectedIndex == 7)
                {
                    window->close();
                    return false;
                }
                else if (selectedIndex == 4)
                {
                    subState = CREDITS;
                    return false;
                }
                else if (selectedIndex == 5)
                {
                    subState = INSTRUCTIONS;
                    return false;
                }
                else if (selectedIndex == 6)
                {
                    subState = SCOREBOARD;
                    return false;
                }
                return (selectedIndex >= 0 && selectedIndex <= 3);
            }
        }

        if (ev.type == sf::Event::MouseButtonPressed)
        {
            if (subState != MAIN)
            {
                subState = MAIN;
                return false;
            }

            for (int i = 0; i < 8; i++)
            {
                if (getGlobalBounds(menuOptions[i]).contains(window->mapPixelToCoords(sf::Mouse::getPosition(*window))))
                {
                    if (i == 7)
                    {
                        window->close();
                        return false;
                    }
                    else if (i == 4)
                    {
                        subState = CREDITS;
                        return false;
                    }
                    else if (i == 5)
                    {
                        subState = INSTRUCTIONS;
                        return false;
                    }
                    else if (i == 6)
                    {
                        subState = SCOREBOARD;
                        return false;
                    }
                    return (i >= 0 && i <= 3);
                }
            }
        }
        return false;
    }


    // Advances state calculations by frame time increment

    void update(float dt)
    {
        if (!window || subState != MAIN)
        {
            return;
        }
        for (int i = 0; i < 8; i++)
        {
            if (getGlobalBounds(menuOptions[i]).contains(window->mapPixelToCoords(sf::Mouse::getPosition(*window))))
            {
                menuOptions[selectedIndex].setFillColor(sf::Color::White); // Color: White
                selectedIndex = i;
                menuOptions[selectedIndex].setFillColor(sf::Color::Yellow); // Color: Yellow
            }
        }
    }

    void render()
    {
        if (!window)
        {
            return;
        }

        if (selectedIndex >= 0 && selectedIndex <= 3)
        {
            if (texStart.getSize().x > 0)
            {
                backgroundSprite.setTexture(texStart);
            }
        }
        else if (selectedIndex == 5) // Instructions
        {
            if (texOptions.getSize().x > 0)
            {
                backgroundSprite.setTexture(texOptions);
            }
        }
        else if (selectedIndex == 4) // Credits
        {
            if (texCredits.getSize().x > 0)
            {
                backgroundSprite.setTexture(texCredits);
            }
        }
        else if (selectedIndex == 7) // Exit
        {
            if (texExit.getSize().x > 0)
            {
                backgroundSprite.setTexture(texExit);
            }
        }
        window->draw(backgroundSprite);

        if (subState == MAIN)
        {
            for (int i = 0; i < 8; i++)
            {
                window->draw(menuOptions[i]);
            }
        }
        else if (subState == CREDITS)
        {
            sf::RectangleShape overlay(MyVector2f(1600.f, 900.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 220)); // Color: Translucent UI Dark Overlay
            window->draw(overlay);

            sf::Text title("CREDITS", menuFont, 50);
            title.setFillColor(sf::Color::Yellow); // Color: Yellow
            title.setStyle(sf::Text::Bold);
            sf::FloatRect bounds = title.getLocalBounds();
            title.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            title.setPosition(800.f, 150.f);
            window->draw(title);

            sf::Text content("DEVELOPED BY: ANTIGRAVITY & USER\n\nASSETS BY: SNK PLAYMORE (METAL SLUG)\n\nFRAMEWORK: C++ & SFML\n\nSPECIAL THANKS TO: GOOGLE DEEPMIND TEAM\n\n\n[PRESS ENTER OR ESCAPE TO RETURN]", menuFont, 25);
            content.setFillColor(sf::Color::White); // Color: White
            bounds = content.getLocalBounds();
            content.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            content.setPosition(800.f, 450.f);
            window->draw(content);
        }
        else if (subState == INSTRUCTIONS)
        {
            sf::RectangleShape overlay(MyVector2f(1600.f, 900.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 220)); // Color:  Dark Overlay
            window->draw(overlay);

            sf::Text title("INSTRUCTIONS & KEYS", menuFont, 50);
            title.setFillColor(sf::Color::Yellow); // Color: Yellow
            title.setStyle(sf::Text::Bold);
            sf::FloatRect bounds = title.getLocalBounds();
            title.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            title.setPosition(800.f, 150.f);
            window->draw(title);

            sf::Text content("MOVE LEFT / RIGHT:  A / D   or   LEFT / RIGHT ARROW KEYS\n\nAIM UP:  W   or   UP ARROW KEY\n\nCROUCH:  S   or   DOWN ARROW KEY\n\nJUMP:  SPACE   or   X KEY\n\nSHOOT:  C   or   F KEY\n\nSWITCH CHARACTER:  Z KEY\n\nDEV MODE:  F1 KEY\n\nGO BACK / MENU:  ESCAPE KEY\n\n\n[PRESS ENTER OR ESCAPE TO RETURN]", menuFont, 24);
            content.setFillColor(sf::Color::White); // Color: White
            bounds = content.getLocalBounds();
            content.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            content.setPosition(800.f, 480.f);
            window->draw(content);
        }
        else if (subState == SCOREBOARD)
        {
            sf::RectangleShape overlay(MyVector2f(1600.f, 900.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 220)); // Color: Dark Overlay
            window->draw(overlay);

            sf::Text title("SCOREBOARD", menuFont, 50);
            title.setFillColor(sf::Color::Yellow); // Color: Yellow
            title.setStyle(sf::Text::Bold);
            sf::FloatRect bounds = title.getLocalBounds();
            title.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            title.setPosition(800.f, 150.f);
            window->draw(title);
            char scoreBuf[64] = "HIGH SCORE: ";
            char tmp[16];
            ScoreManager::intToStr(GameData::highScore, tmp, 16);
            ScoreManager::strAppend(scoreBuf, 64, tmp);

            sf::Text content(scoreBuf, menuFont, 40);
            content.setFillColor(sf::Color::Green); // Color: Green
            bounds = content.getLocalBounds();
            content.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            content.setPosition(800.f, 400.f);
            window->draw(content);

            sf::Text prompt("[PRESS ENTER OR ESCAPE TO RETURN]", menuFont, 25);
            prompt.setFillColor(sf::Color::White); // Color: White
            bounds = prompt.getLocalBounds();
            prompt.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            prompt.setPosition(800.f, 600.f);
            window->draw(prompt);
        }
    }

    int getSelectedIndex() const 
    {
        return selectedIndex;
    }
    int getSubState() const 
    {
        return subState;
    }
};

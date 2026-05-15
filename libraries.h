#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

// Enemy Manager

class EnemyManager;

// Score Manager

class ScoreManager;

struct GameData {
    static int currentLevel;
    static int highScore;
    static bool campaignMode;
    static EnemyManager* activeEnemyManager;
    static ScoreManager* activeScoreManager;
};

struct MyVector2f {
    float x, y;
    MyVector2f(float x = 0, float y = 0) : x(x), y(y) {}
    MyVector2f(const sf::Vector2f& other) : x(other.x), y(other.y) {}
    operator sf::Vector2f() const 
{
    return sf::Vector2f(x, y);
}
};

struct MyVector2u {
    unsigned int x, y;
    MyVector2u(unsigned int x = 0, unsigned int y = 0) : x(x), y(y) {}
    MyVector2u(const sf::Vector2u& other) : x(other.x), y(other.y) {}
    operator sf::Vector2u() const 
{
    return sf::Vector2u(x, y);
}
};


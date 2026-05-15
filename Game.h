#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>



class GameState;

class GameStateManager;

class GameStateManager
{
private:
    GameState** m_states; 
    int maxSize; 
    int maxCapacity; 
    void resize(); 

public:
    GameStateManager();
    ~GameStateManager();

    void pushState(GameState* state);
    void popState();
    void changeState(GameState* state);

    GameState* getCurrentState() const;
    bool isEmpty() const;

};

// Game State

class GameState

{
protected:
    GameStateManager* gsm;
    sf::RenderWindow* window;

public:
    GameState(GameStateManager* gsm, sf::RenderWindow* window)
        :gsm(gsm), window(this->window) 

{

}
    virtual ~GameState() = default;

    virtual void handleEvent(const sf::Event& ev) = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;

    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void onPause() = 0;
    virtual void onResume() = 0;
};

// Game

class Game
{

private:
    void processEvents();

    void update(float dt);
    void render();

    sf::RenderWindow window;
    GameStateManager gsm;
    sf::Clock clock;

public:
    Game();
    ~Game();

    void run();

    sf::RenderWindow& getWindow() 

{

    return window;

}
    GameStateManager& getGSM() 

{

    return gsm;

}
    static const int SCREEN_W = 1600;
    static const int SCREEN_H = 900;
    static const int TARGET_FPS = 60;


};

#include "Game.h"     
#include "libraries.h" 

GameStateManager::GameStateManager()



{
    maxCapacity = 8;
    maxSize = 0;
    m_states = new GameState * [maxCapacity];


    for (int i = 0; i < maxCapacity; i++)
        m_states[i] = NULL;

}

GameStateManager::~GameStateManager()


{
    for (int i = 0; i < maxSize; i++)
    

{
        delete m_states[i];
        m_states[i] = NULL;
    }
    delete[] m_states;

}

void GameStateManager::resize()
{
    int newCap = maxCapacity + 1;         // new capacity is increased by 1
    GameState** temp = new GameState * [newCap];  // allocate a brand new bigger array



    for (int i = 0; i < maxSize; i++)
    

{
    

    temp[i] = m_states[i];
    

}
    temp[maxSize] = NULL;


    delete[] m_states;
    m_states = temp;
    maxCapacity = newCap;
}

void GameStateManager::pushState(GameState* state)



{
    if (state == NULL)
    

{
    

    return;
    

}
    if (maxSize >= maxCapacity)
    

{
    

    resize();
    

}
    if (maxSize > 0)
        m_states[maxSize - 1]->onPause();

    m_states[maxSize] = state;
    maxSize++;
    state->onEnter();
}

void GameStateManager::popState()



{
    if (isEmpty())
    

{
    

    return;
    

}
    m_states[maxSize - 1]->onExit();
    delete m_states[maxSize - 1];
    m_states[maxSize - 1] = NULL;
    maxSize--;

    if (maxSize > 0)
        m_states[maxSize - 1]->onResume();

}

void GameStateManager::changeState(GameState* state)


{
    if (!isEmpty())
    

{
    

    popState();
    

}
    pushState(state);
}

GameState* GameStateManager::getCurrentState() const


{
    if (isEmpty())
    

{
    

    return NULL;
    

}
    return m_states[maxSize - 1];
}

bool GameStateManager::isEmpty() const



{



    return maxSize == 0;



}
Game::~Game() 

{

}
Game::Game()
    : window(sf::VideoMode(SCREEN_W, SCREEN_H), "Metal Slug", sf::Style::Close)



{
    window.setFramerateLimit(TARGET_FPS);   // tell SFML to cap the loop at 60 frames per second
    window.setVerticalSyncEnabled(true);    // sync to monitor refresh rate to prevent screen tearing

}


void Game::run()


{
    while (window.isOpen())
    

{
        float dt = clock.restart().asSeconds();
        // clock.restart() does two things:
        // dt = "delta time" = time between this frame and the last frame

        if (dt > 0.05)
        

{
        

    dt = 0.05;
        

}

        processEvents(); //read all input events (keyboard, mouse, window close)
        update(dt);       // move things, apply physics, run game logic
        render();         // draw everything to screen
    }
}

void Game::processEvents()



{
    sf::Event ev;               // ev is a container that holds one event at a time
    while (window.pollEvent(ev))
    {
        if (ev.type == sf::Event::Closed)   // user clicked the X button on the window
            window.close();                  // this causes isOpen() to return false next frame

        if (!gsm.isEmpty())                           // only forward event if a state is loaded
            gsm.getCurrentState()->handleEvent(ev);   // let the active state handle this event
    }
}


// Advances state calculations by frame time increment

void Game::update(float dt)



{
    if (!gsm.isEmpty())
        gsm.getCurrentState()->update(dt);
}

void Game::render()



{
    window.clear();

    if (!gsm.isEmpty())
        gsm.getCurrentState()->render();


    window.display();
}






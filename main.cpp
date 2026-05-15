#include "libraries.h"
#include "World.h"
#include "Camera.h"
#include "SpriteRendering.h"
#include "Soldier.h"
#include "Enemy.h"
#include "Weapon_Types.h"
#include "mainmenu.h"
#include "GameBackground.h"
#include "ScoreManager.h"
#include "Vehicle.h"
#include "Mouse.h"

using namespace sf;

static const int SW = 1600;
static const int SH = 900;
static const float PW = 90.f;
static const float PH = 145.f;

int GameData::currentLevel = 1;
bool GameData::campaignMode = false;

static int findSurface(const World& w, int bx)


{
    for (int by = 0; by < WORLD_H; by++)
    

{
        if (w.getBlock(bx, by) == GRASS)
        

{
        

    return by;
        

}
    }
    return WORLD_H / 2;
}

static void resolveH(Soldier& s, const World& w)


{
    int byT = (int)(s.getY() / CELL);
    int byB = (int)((s.getY() + PH - 1.f) / CELL);
    int bxL = (int)(s.getX() / CELL);
    int bxR = (int)((s.getX() + PW - 1.f) / CELL);

    if (s.getVelX() < 0)
    

{
        for (int by = byT; by <= byB; by++)
        

{
            if (w.isSolid(bxL, by))
            

{
                s.setPosition((float)((bxL + 1) * CELL), s.getY());
                s.setVelX(0);
                break;
            }
        }
    }
    if (s.getVelX() > 0)
    

{
        for (int by = byT; by <= byB; by++)
        

{
            if (w.isSolid(bxR, by))
            

{
                s.setPosition((float)(bxR * CELL) - PW, s.getY());
                s.setVelX(0);
                break;
            }
        }
    }
}

static void resolveV(Soldier& s, const World& w, bool drop)


{
    int bxL = (int)(s.getX() / CELL);
    int bxR = (int)((s.getX() + PW - 1.f) / CELL);
    int byTop = (int)(s.getY() / CELL);

    if (s.getVelY() >= 0)
    

{
        int byFeet = (int)((s.getY() + PH) / CELL);
        for (int bx = bxL; bx <= bxR; bx++)
        

{
            if (w.isSolid(bx, byFeet))
            

{
                s.setPosition(s.getX(), (float)(byFeet * CELL) - PH);
                s.setVelY(0);
                s.setOnGround(true);
                return;
            }
            if (!drop && w.isOneWay(bx, byFeet))
            

{
                float platTop = (float)(byFeet * CELL);
                if (s.getY() + PH <= platTop + 4.f)
                

{
                    s.setPosition(s.getX(), platTop - PH);
                    s.setVelY(0);
                    s.setOnGround(true);
                    return;
                }
            }
        }
        int byFeetPx = (int)((s.getY() + PH - 1.f) / CELL);
        if (byFeetPx != byFeet)
        

{
            for (int bx = bxL; bx <= bxR; bx++)
            

{
                if (w.isSolid(bx, byFeetPx))
                

{
                    s.setPosition(s.getX(), (float)(byFeetPx * CELL) - PH);
                    s.setVelY(0);
                    s.setOnGround(true);
                    return;
                }
            }
        }
    }

    if (s.getVelY() < 0)
    

{
        for (int bx = bxL; bx <= bxR; bx++)
        

{
            if (w.isSolid(bx, byTop))
            

{
                s.setPosition(s.getX(), (float)((byTop + 1) * CELL));
                s.setVelY(0);
                return;
            }
        }
    }
}

static void itosbuf(int v, char* buf, int cap)


{
    int i = 0;
    if (v < 0 && i < cap - 1)
    

{
    

    buf[i++] = '-';
    v = -v;
    

}
    if (v == 0 && i < cap - 1)
    

{
    

    buf[i++] = '0';
    

}
    else
    

{
    char tmp[16];
    int t = 0;
        while (v > 0 && t < 15)
        

{
        

    tmp[t++] = '0' + (v % 10);
    v /= 10;
        

}
        while (t > 0 && i < cap - 1)
        

{
        

    buf[i++] = tmp[--t];
        

}
    }
    buf[i] = '\0';
}

static void sappend(char* dst, int cap, const char* src)


{
    int i = 0;
    while (dst[i] && i < cap - 1)
    

{
    

    i++;
    

}
    int j = 0;
    while (src[j] && i < cap - 1)
    

{
    

    dst[i++] = src[j++];
    

}
    dst[i] = '\0';
}

static void drawHUD(RenderWindow& win, Font& font, const Soldier& s, bool dev)


{
    char buf[256];
    buf[0] = '\0';
    char tmp[32];

    sappend(buf, 256, "Pos:(");
    itosbuf((int)(s.getX() / CELL), tmp, 32);
    sappend(buf, 256, tmp);
    sappend(buf, 256, ",");
    itosbuf((int)(s.getY() / CELL), tmp, 32);
    sappend(buf, 256, tmp);
    sappend(buf, 256, ")  HP:");
    itosbuf(s.getHP(), tmp, 32);
    sappend(buf, 256, tmp);
    sappend(buf, 256, "  Grenades:");
    itosbuf(s.getGrenades(), tmp, 32);
    sappend(buf, 256, tmp);
    bool inWater = (s.getX() >= 2080.f && s.getX() <= 4000.f && s.getY() >= 500.f);
    if (inWater) 

{

    sappend(buf, 256, "  IN WATER");

}
    else 

{
    if (s.isOnGround())
    
{
    
    sappend(buf, 256, "  GROUNDED");
    
}
    else
    
{
    
    sappend(buf, 256, "  AIRBORNE");
    
}
}
    if (dev)
    

{
    

    sappend(buf, 256, "  [DEV]");
    

}
    Text info;
    info.setFont(font);
    info.setString(buf);
    info.setCharacterSize(18);
    info.setFillColor(Color::Cyan);
    info.setPosition(8.f, 6.f);
    win.draw(info);

    Text ctrl;
    ctrl.setFont(font);
    ctrl.setString("Left/Right:Move  Up/Space:Jump  Down:Drop  S:Shoot  H:Weapon  Z:Switch  F1:Dev");
    ctrl.setCharacterSize(15);
    ctrl.setFillColor(Color(255, 255, 180));
    ctrl.setPosition(8.f, (float)(SH - 24));
    win.draw(ctrl);

    Text charName;
    charName.setFont(font);
    charName.setString(s.getName());
    charName.setCharacterSize(20);
    charName.setFillColor(Color::Yellow);
    charName.setPosition(8.f, 30.f);
    win.draw(charName);
    Text lvlTxt;
    lvlTxt.setFont(font);
    lvlTxt.setString("Level: " + std::to_string(GameData::currentLevel) + " / 3");
    lvlTxt.setCharacterSize(22);
    lvlTxt.setFillColor(Color::White);
    lvlTxt.setStyle(Text::Bold);
    lvlTxt.setPosition((float)(SW - 150), 10.f);
    win.draw(lvlTxt);
}

int main()


{
    RenderWindow window(VideoMode(SW, SH), "Metal Slug - Procedural World", Style::Close);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    Font font;
    bool fontOK = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontOK)
    

{
    

    fontOK = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    

}
    SpriteRenderer sr;
    World* world = new World(1234, 2);
    BlockTextures blockTex = sr.getBlockTextures();

    GameBackground bg;
    bg.load(1);

    ScoreManager scoreMgr;
    float transitionTimer = 0;
    bool transitioning = false;

    int surfRow = findSurface(*world, 4);
    float spawnX = 4.f * CELL;
    float spawnY = (float)(surfRow * CELL) - PH;

    Marco marco(spawnX, spawnY);
    Eri eri(spawnX, spawnY);
    Fio fio(spawnX, spawnY);
    Tarma tarma(spawnX, spawnY);
    Soldier* players[4] = { &marco, &eri, &fio, &tarma };
    int currentPlayer = 0;
    Soldier* player = players[0];
    player->setOnGround(true);

    Camera cam;
    cam.follow(spawnX + PW / 2.f, spawnY + PH / 2.f);
    cam.snap();

    EnemyManager enemyMgr;


    // SPAWN VEHICLES IN BIOMES
    MetalSlugTank testSlug(1200.f, 500.f);
    SlugFlyer testFlyer(4500.f, 200.f);
    SlugMariner testMariner(2500.f, 750.f);

    Vehicle* currentVehicle = NULL;

    bool devMode = false;
    bool inMenu = true;
    Clock clock;

    Sprite blockSprite;
    Sprite playerSprite;
    Sprite eyeSprite;
    Sprite overlaySprite;
    Sprite hillSprite;

    hillSprite.setTexture(sr.texHill);

    MainMenuState menu(NULL, &window);

    while (window.isOpen())
    

{
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f)
        

{
        

    dt = 0.05f;
        

}
        Event ev;
        while (window.pollEvent(ev))
        

{
            if (ev.type == Event::Closed)
            

{
            

    window.close();
            

}
            if (inMenu)
            

{
                // Dispatches menu selection results confirming game start state
                if (menu.handleEvent(ev))
                

{
                    inMenu = false;
                    int sel = menu.getSelectedIndex();
                    delete world;
                    
                    GameData::campaignMode = (sel == 3);

                    if (sel == 1)
                    {
                        GameData::currentLevel = 2;
                        bg.load(2);
                        world = new World(5678, 2);
                    }
                    else if (sel == 2)
                    {
                        GameData::currentLevel = 3;
                        bg.load(3);
                        world = new World(9999, 0);
                    }
                    else if (sel == 3)
                    {
                        GameData::currentLevel = 1;
                        bg.load(1);
                        world = new World(rand() % 99999, 2);
                    }
                    else
                    {
                        GameData::currentLevel = 1;
                        bg.load(1);
                        world = new World(1234, 2);
                    }
                    enemyMgr = EnemyManager();
                    
                    int newSurface = findSurface(*world, 4);
                    float nPX = 4.f * CELL;
                    float nPY = (float)(newSurface * CELL) - PH;
                    for (int pix = 0; pix < 4; pix++) 
                    {
                        players[pix]->setPosition(nPX, nPY);
                        players[pix]->setVelX(0);
                        players[pix]->setVelY(0);
                    }
                    float bestX_Slug = 1200.f;
                    float bestX_Mar = 2500.f;
                    float bestX_Fly = 4500.f;

                    if (GameData::campaignMode)
                    {
                        for (int bScan = 30; bScan < 1000; bScan += 10)
                        {
                            if (world->getBiomeAt(bScan) == 0) { bestX_Slug = (float)(bScan * CELL); break; }
                        }
                        for (int bScan = 30; bScan < 1000; bScan += 10)
                        {
                            if (world->getBiomeAt(bScan) == 1) { bestX_Mar = (float)(bScan * CELL); break; }
                        }
                        for (int bScan = 30; bScan < 1000; bScan += 10)
                        {
                            if (world->getBiomeAt(bScan) == 2) { bestX_Fly = (float)(bScan * CELL); break; }
                        }
                    }

                    // Dynamically scans current map layer and relocates asset safely
                    int vSurfSlug = findSurface(*world, (int)(bestX_Slug / CELL));
                    testSlug.setPosition(bestX_Slug, (float)(vSurfSlug * CELL) - 100.f);

                    int vSurfMar = findSurface(*world, (int)(bestX_Mar / CELL));
                    testMariner.setPosition(bestX_Mar, (float)(vSurfMar * CELL) - 100.f);

                    int vSurfFly = findSurface(*world, (int)(bestX_Fly / CELL));
                    testFlyer.setPosition(bestX_Fly, (float)(vSurfFly * CELL) - 300.f);

                    
                    cam.follow(nPX + PW / 2.f, nPY + PH / 2.f);
                    cam.snap();
                }
            }
            else
            

{
                if (ev.type == Event::MouseButtonPressed && ev.mouseButton.button == Mouse::Left)
                

{
                    float clickX = ev.mouseButton.x + cam.x;
                    float clickY = ev.mouseButton.y + cam.y;

                    float spawnX;
                    if (player->isFacingRight())
                    
{
                    
    spawnX = player->getX() + 75;
                    
}
                    else
                    
{
                    
    spawnX = player->getX() + 15;
                    
}
                    float spawnY = player->getY() + 68.f;

                    float dx = clickX - spawnX;
                    float dy = clickY - spawnY;
                    float angleRad = atan2(dy, dx);

                    player->setFacingRight(dx >= 0);

                    int count = 0;
                    Projectile** projs = player->shoot(angleRad, count);
                    if (projs) 

{

    delete[] projs;

}
                }

                if (ev.type == Event::KeyPressed)
                

{
                    if (ev.key.code == Keyboard::Escape)
                    

{
                    

    inMenu = true;
                    

}
                    if (ev.key.code == Keyboard::F1)
                    

{
                    

    devMode = !devMode;
                    

}
                    if (ev.key.code == Keyboard::Z)
                    

{
                        if (player->getTransformState() != TransformationState::mummy)
                        

{
                            int nextPlayer = -1;
                            for (int offset = 1; offset <= 4; offset++)
                            

{
                                int idx = (currentPlayer + offset) % 4;
                                if (players[idx]->getLives() > 0)
                                

{
                                    nextPlayer = idx;
                                    break;
                                }
                            }

                            if (nextPlayer != -1 && nextPlayer != currentPlayer)
                            

{
                                players[nextPlayer]->setPosition(player->getX(), player->getY());
                                players[nextPlayer]->setOnGround(player->isOnGround());
                                currentPlayer = nextPlayer;
                                player = players[currentPlayer];
                            }
                        }
                    }

                    if (ev.key.code == Keyboard::E)
                    

{
                        if (currentVehicle)
                        

{
                            currentVehicle->eject();
                            player->setPosition(currentVehicle->getX(), currentVehicle->getY() - PH);
                            player->setVelY(-200.f);
                            currentVehicle = NULL;
                        }
                        else
                        

{
                            Vehicle* pool[] = { &testSlug, &testFlyer, &testMariner };
                            for (int i = 0; i < 3; i++)
                            

{
                                sf::FloatRect vb = pool[i]->getBounds();
    vb.left -= 50.f;
    vb.width += 100.f;
                                if (vb.contains(player->getX() + PW / 2.f, player->getY() + PH / 2.f))
                                

{
                                    pool[i]->board(player);
                                    currentVehicle = pool[i];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (inMenu)
        

{
            menu.update(dt);
            window.clear();
            menu.render();
            window.display();
        }
        else
        

{
            if (transitioning)
            

{
                transitionTimer -= dt;
                if (transitionTimer <= 0)
                

{
                    transitioning = false;
                    GameData::currentLevel++;
                    bg.load(GameData::currentLevel);

                    delete world;
                    if (GameData::currentLevel == 2)
                    {
                        world = new World(5678, 2);
                    }
                    else if (GameData::currentLevel == 3) world = new World(9999, 0); // Ensure L3 differs wildly!
                    else world = new World(1234, 2);

                    for (int i = 0; i < 4; i++)
                    

{
                        players[i]->setPosition(spawnX, spawnY);
                        players[i]->setOnGround(true);
                        players[i]->setVelX(0);
                        players[i]->setVelY(0);
                    }
                    int activePlayer = 0;
                    for (int i = 0; i < 4; i++)
                    

{
                        if (players[i]->getLives() > 0)
                        

{
                            activePlayer = i;
                            break;
                        }
                    }
                    currentPlayer = activePlayer;
                    player = players[currentPlayer];

                    enemyMgr = EnemyManager();
                    
                    int newSurface = findSurface(*world, 4);
                    float nPX = 4.f * CELL;
                    float nPY = (float)(newSurface * CELL) - PH;
                    for (int pix = 0; pix < 4; pix++) 
                    {
                        players[pix]->setPosition(nPX, nPY);
                        players[pix]->setVelX(0);
                        players[pix]->setVelY(0);
                    }
                    // Dynamically scans current map layer and relocates asset safely
                    int vSurfSlug = findSurface(*world, (int)(1200.f / CELL));
                    testSlug.setPosition(1200.f, (float)(vSurfSlug * CELL) - 80.f);
                    
                    int vSurfMar = findSurface(*world, (int)(2500.f / CELL));
                    testMariner.setPosition(2500.f, (float)(vSurfMar * CELL) - 80.f);
                    
                    int vSurfFly = findSurface(*world, (int)(4500.f / CELL));
                    testFlyer.setPosition(4500.f, (float)(vSurfFly * CELL) - 250.f);
                    
                    cam.follow(nPX + PW / 2.f, nPY + PH / 2.f);
                    cam.snap();
                    cam.follow(spawnX + PW / 2.f, spawnY + PH / 2.f);
                    cam.snap();
                }
            }
            else
            

{
                bool drop = Keyboard::isKeyPressed(Keyboard::Down);
                bool wasOnGround = player->isOnGround();

                if (currentVehicle)
                

{
                    currentVehicle->handleInput(dt);
                    currentVehicle->update(dt);

                    if (currentVehicle == &testSlug)
                    

{
                        int bx = (int)(testSlug.getX() / CELL);
                        int by = (int)((testSlug.getY()) / CELL);
                        if (world->isSolid(bx, by) || world->isOneWay(bx, by))
                        

{
                            testSlug.setPosition(testSlug.getX(), (float)(by * CELL));
                            testSlug.setOnGround(true);
                            testSlug.setVelocityY(0);
                        }
                        else
                        

{
                        

    testSlug.setOnGround(false);
                        

}
                    }
                }
                else
                

{
                    player->handleInput(dt);
                    MouseControl::handleContinuousFiring(player, window, cam, dt);
                    resolveH(*player, *world);
                    player->setOnGround(false);
                    resolveV(*player, *world, drop && wasOnGround);
                }

                player->update(dt);
                if (currentVehicle)
                {
                    player->setPosition(currentVehicle->getX() - PW / 2.f, currentVehicle->getY() - PH / 2.f);
                    player->setVelX(0);
                    player->setVelY(0);
                }

                if (player->isDead() && player->getLives() <= 0)
                

{
                    int nextPlayer = -1;
                    for (int offset = 1; offset <= 4; offset++)
                    

{
                        int idx = (currentPlayer + offset) % 4;
                        if (players[idx]->getLives() > 0)
                        

{
                            nextPlayer = idx;
                            break;
                        }
                    }
                    if (nextPlayer != -1)
                    

{
                        players[nextPlayer]->setPosition(player->getX(), player->getY());
                        players[nextPlayer]->setOnGround(player->isOnGround());
                        currentPlayer = nextPlayer;
                        player = players[currentPlayer];
                    }
                    else { inMenu = true; GameData::currentLevel = 1; for (int i = 0; i < 4; i++) { players[i]->reset(); players[i]->setPosition(spawnX, spawnY); players[i]->setOnGround(true); } currentPlayer = 0; player = players[currentPlayer]; }
                }

                // Ensure background updates process
                world->update(cam.x, dt);
                cam.follow(player->getX() + PW / 2.f, player->getY() + PH / 2.f);
                cam.update(dt);

                if (GameData::currentLevel == 3)
                

{
                

    enemyMgr.updateLevel3(dt, player, *world);
                

}
                else if (GameData::currentLevel == 2)
                

{
                

    enemyMgr.updateLevel2(dt, player, *world);
                

}
                else
                

{
                

    enemyMgr.update(dt, player, *world);
                

}
                if (currentVehicle != &testSlug)
                {
                    testSlug.update(dt);
                }
                if (currentVehicle != &testFlyer)
                {
                    testFlyer.update(dt);
                }
                if (currentVehicle != &testMariner)
                {
                    testMariner.update(dt);
                }

                if (currentVehicle != &testSlug) 

{
                    int bx = (int)(testSlug.getX() / CELL);
                    int by = (int)(testSlug.getY() / CELL);
                    if (world->isSolid(bx, by) || world->isOneWay(bx, by)) 

{
                        testSlug.setPosition(testSlug.getX(), (float)(by * CELL));
                        testSlug.setOnGround(true);
                    }
                    else 

{
                        testSlug.setOnGround(false);
                        testSlug.update(dt); // extra passive fall
                    }
                }
                scoreMgr.update(dt);

                if (GameData::currentLevel == 1 && enemyMgr.getSpawnWaves() >= 7 && enemyMgr.getEnemyCount() == 0)
                

{
                    transitioning = true;
                    transitionTimer = 3.f;
                }

                if (GameData::currentLevel == 2 && player->getX() >= 5000.f && enemyMgr.getEnemyCount() == 0)
                

{
                    transitioning = true;
                    transitionTimer = 3.f;
                }

                if (GameData::currentLevel == 3 && player->getX() >= 5000.f && enemyMgr.getEnemyCount() == 0)
                

{
                    inMenu = true;
                    GameData::currentLevel = 1;
                    bg.load(1);
                    for (int i = 0; i < 4; i++)
                    

{
                        players[i]->setPosition(spawnX, spawnY);
                        players[i]->setOnGround(true);
                    }
                    int activePlayer = 0;
                    for (int i = 0; i < 4; i++)
                    

{
                        if (players[i]->getLives() > 0)
                        

{
                            activePlayer = i;
                            break;
                        }
                    }
                    currentPlayer = activePlayer;
                    player = players[currentPlayer];
                }
            }

            window.clear(Color(100, 160, 220));

            if (GameData::currentLevel == 1)
            

{
                if (sr.texHill.getSize().x > 1)
                

{
                    int bgW = sr.texHill.getSize().x;
                    int startRep = (int)(cam.x * 0.5f / bgW) - 1;
                    for (int i = startRep; i < startRep + 3; i++)
                    

{
                        hillSprite.setScale(1.f, 1.f);
                        hillSprite.setPosition((float)(i * bgW) - cam.x * 0.5f, 0);
                        window.draw(hillSprite);
                    }
                }
                else
                

{
                    // Fallback procedural hills
                    for (int i = -1; i < 10; i++)
                    

{
                        float val = cam.x * 0.25f;
                        int q = (int)(val / 240.f);
                        float rem = val - (float)(q * 240);
                        if (rem < 0)
                        

{
                        

    rem += 240.f;
                        

}
                        float hx = (float)(i * 240) - rem;
                        hillSprite.setScale(200.f, 130.f);
                        hillSprite.setPosition(hx, (float)SH * 0.44f);
                        window.draw(hillSprite);
                    }
                }
            }
            else
            

{
            

    bg.render(window, cam.x, SW, SH);
            

}
            world->render(window, blockSprite, blockTex, cam.x, cam.y, SW, SH);

            if (!currentVehicle)
            

{
            

    player->render(window, cam.x, cam.y);
            

}
            // Render Vehicles
            testSlug.render(window, cam.x, cam.y);
            testFlyer.render(window, cam.x, cam.y);
            testMariner.render(window, cam.x, cam.y);

            if (fontOK)
            

{
                sf::Text playerHPText;
                playerHPText.setFont(font);
                playerHPText.setString("HP: " + std::to_string(player->getHP()));
                playerHPText.setCharacterSize(16);
                playerHPText.setFillColor(sf::Color::Green); // Color: Green
                playerHPText.setStyle(sf::Text::Bold);
                playerHPText.setPosition(player->getX() - cam.x + 25.f, player->getY() - cam.y - 25.f);
                window.draw(playerHPText);
            }

            // Render enemies
            enemyMgr.render(window, cam.x, cam.y);

            if (fontOK)
            

{
                scoreMgr.renderPopups(window, font, cam.x, cam.y);
                scoreMgr.renderHUD(window, font, SW);
            }

            int dmg = player->getDamageState();
            if (player->getHitWindowTimer() > 0)
            

{
                if (dmg == DamagableEntity::injuredState)
                

{
                    overlaySprite.setTexture(sr.texRedLight);
                    overlaySprite.setScale((float)SW, (float)SH);
                    overlaySprite.setPosition(0, 0);
                    window.draw(overlaySprite);
                }
                else if (dmg == DamagableEntity::criticalState)
                

{
                    overlaySprite.setTexture(sr.texRedHeavy);
                    overlaySprite.setScale((float)SW, (float)SH);
                    overlaySprite.setPosition(0, 0);
                    window.draw(overlaySprite);
                }
            }

            if (fontOK)
            

{
            

    drawHUD(window, font, *player, devMode);
            

}
            if (transitioning)
            

{
                sf::Text transText;
                transText.setFont(font);
                transText.setString("MISSION " + std::to_string(GameData::currentLevel) + " ACCOMPLISHED!\n\nPREPARING MISSION " + std::to_string(GameData::currentLevel + 1));
                transText.setCharacterSize(35);
                transText.setFillColor(sf::Color::Yellow); // Color: Yellow

                float estWidth = 31 * 35.f * 0.45f;
                transText.setPosition((float)SW / 2.f - estWidth / 2.f, (float)SH / 2.f - 35.f);
                window.draw(transText);
            }

            window.display();
        }
    }

    delete world;
    return 0;
}







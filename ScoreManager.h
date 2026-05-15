#pragma once
#include <SFML/Graphics.hpp>
#include "Enemy.h"

struct ScorePopup
{
    float x, y;           // world-space spawn position
    float velY;           
    float lifetime;       // counts DOWN to 0
    float maxLife;
    bool active;
    char  text[40];       
    int value;

    ScorePopup()
        : x(0), y(0), velY(-55.f), lifetime(0), maxLife(1.6f), active(false)
    

{
    

    text[0] = '\0';
    

}
};

// Score Manager

class ScoreManager
{
public:
    static const int PTS_REBEL = 100;
    static const int PTS_SHIELDED = 200;
    static const int PTS_BAZOOKA = 300;
    static const int PTS_GRENADE_SOL = 300;
    static const int PTS_ZOMBIE = 250;
    static const int PTS_MUMMY = 400;
    static const int PTS_MARTIAN = 500;

    static const int BONUS_MULTI_KILL = 500;   // 2+ kills within 0.8 s
    static const int BONUS_NO_DAMAGE = 1000;  // wave cleared without being hit

    static const int COMBO_FOR_X2 = 3;
    static const int COMBO_FOR_X3 = 6;
    static const int COMBO_FOR_X4 = 10;

    static const float COMBO_RESET_SECS;   
    static const float MULTI_KILL_WINDOW;  

    ScoreManager();

    void onEnemyKilled(int enemyType, float worldX, float worldY);

    void onPlayerDamaged();

    void awardBonus(int pts, const char* label, float worldX, float worldY);

 
    void update(float dt);

    void renderPopups(sf::RenderWindow& window, sf::Font& font,
        float camX, float camY) const;

    void renderHUD(sf::RenderWindow& window, sf::Font& font, int screenW) const;

    void resetWave();

    int getScore() const 

{

    return score;

}
    int getKills() const 

{

    return totalKills;

}
    int getCombo() const 

{

    return comboStreak;

}
    int getMultiplier() const 

{

    return multiplier;

}
private:
    int score;
    int totalKills;
    int comboStreak;       // kills without the timer expiring
    int multiplier;        // 1 / 2 / 3 / 4
    float comboTimer;        // time since last kill; reset resets combo
    int multiKillCount;    // kills within MULTI_KILL_WINDOW
    float multiKillTimer;    // counts down; reset on each kill
    bool damageTakenThisWave;

    // Popup pool
    static const int MAX_POPUPS = 32;
    ScorePopup popups[MAX_POPUPS];

    // Internal helpers
    int basePointsFor(int enemyType) const;
    void spawnPopup(float wx, float wy, int pts, const char* extra);
    void recalcMultiplier();

    // No-STL string helpers
public:
    static void intToStr(int v, char* buf, int cap);
    static void strAppend(char* dst, int cap, const char* src);
    static int strLen(const char* s);
    static void strCopy(char* dst, int cap, const char* src);
};






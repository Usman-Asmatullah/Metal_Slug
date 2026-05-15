#include "libraries.h"
#include "ScoreManager.h"
#include <cmath>   

// Global score manager pointer
ScoreManager* GameData::activeScoreManager = NULL;
int GameData::highScore = 0;

const float ScoreManager::COMBO_RESET_SECS = 3.0f;
const float ScoreManager::MULTI_KILL_WINDOW = 0.8f;

ScoreManager::ScoreManager()
    : score(0), totalKills(0), comboStreak(0), multiplier(1),
    comboTimer(0), multiKillCount(0), multiKillTimer(0),
    damageTakenThisWave(false)


{


    GameData::activeScoreManager = this;


}
// onEnemyKilled
void ScoreManager::onEnemyKilled(int enemyType, float worldX, float worldY)


{
    comboStreak++;
    comboTimer = COMBO_RESET_SECS;  // restart window
    recalcMultiplier();

    multiKillCount++;
    if (multiKillTimer <= 0)
        multiKillTimer = MULTI_KILL_WINDOW;   // open a fresh window

    int base = basePointsFor(enemyType);
    int earned = base * multiplier;
    score += earned;
    totalKills++;

    char label[40];
    label[0] = '\0';
    char tmp[16];


    strAppend(label, 40, "+");
    intToStr(earned, tmp, 16);
    strAppend(label, 40, tmp);

    if (multiplier >= 2)
    

{
        strAppend(label, 40, "  x");
        intToStr(multiplier, tmp, 16);
        strAppend(label, 40, tmp);
        strAppend(label, 40, "!");
    }

    spawnPopup(worldX, worldY, earned, label);
}

// onPlayerDamaged
void ScoreManager::onPlayerDamaged()


{
    damageTakenThisWave = true;

    comboStreak = 0;
    comboTimer = 0;
    multiplier = 1;
}

// awardBonus
void ScoreManager::awardBonus(int pts, const char* label, float worldX, float worldY)


{
    score += pts;
    char buf[40];
    buf[0] = '\0';
    char tmp[16];
    strAppend(buf, 40, "+");
    intToStr(pts, tmp, 16);
    strAppend(buf, 40, tmp);
    strAppend(buf, 40, " ");
    strAppend(buf, 40, label);
    spawnPopup(worldX, worldY, pts, buf);
}




void ScoreManager::update(float dt)


{
    if (comboTimer > 0)
    

{
        comboTimer -= dt;
        if (comboTimer <= 0)
        

{
            comboTimer = 0;
            comboStreak = 0;
            multiplier = 1;
        }
    }

    if (multiKillTimer > 0)
    

{
        multiKillTimer -= dt;
        if (multiKillTimer <= 0)
        

{
            if (multiKillCount >= 2)
            

{
                score += BONUS_MULTI_KILL;
                spawnPopup(400.f, 200.f, BONUS_MULTI_KILL, "+500 MULTI-KILL!");
            }
            multiKillCount = 0;
            multiKillTimer = 0;
        }
    }

    for (int i = 0; i < MAX_POPUPS; i++)
    

{
        if (!popups[i].active)
        {
            continue;
        }
        popups[i].y += popups[i].velY * dt;
        popups[i].lifetime -= dt;
        if (popups[i].lifetime <= 0)
            popups[i].active = false;
    }

    if (score > GameData::highScore)
    

{
    

    GameData::highScore = score;
    

}
}

void ScoreManager::renderPopups(sf::RenderWindow& window, sf::Font& font,
    float camX, float camY) const


{
    sf::Text t;
    t.setFont(font);
    t.setCharacterSize(20);

    for (int i = 0; i < MAX_POPUPS; i++)
    

{
        if (!popups[i].active)
        {
            continue;
        }

        float alpha = popups[i].lifetime / popups[i].maxLife;
        unsigned char a = (unsigned char)(alpha * 255.f);

        sf::Color col;
        if (popups[i].value >= 500)
            col = sf::Color(255, 80, 0, a);     // orange
        else if (popups[i].value >= 200)
            col = sf::Color(255, 220, 0, a);    // yellow
        else
            col = sf::Color(255, 255, 255, a);  // white

        t.setFillColor(col);
        t.setString(popups[i].text);
        t.setPosition(popups[i].x - camX, popups[i].y - camY);
        window.draw(t);
    }
}

void ScoreManager::renderHUD(sf::RenderWindow& window, sf::Font& font,
    int screenW) const


{
    char buf[64];
    char tmp[16];
    buf[0] = '\0';
    strAppend(buf, 64, "SCORE: ");
    intToStr(score, tmp, 16);
    strAppend(buf, 64, tmp);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString(buf);
    scoreText.setCharacterSize(22);
    scoreText.setFillColor(sf::Color::White); // Color: White

    int len = 0;
    while (buf[len])
    {
        len++;
    }
    float estWidth = len * 22.f * 0.45f;
    scoreText.setPosition((float)(screenW) - estWidth - 12.f, 6.f);
    window.draw(scoreText);

    buf[0] = '\0';
    strAppend(buf, 64, "KILLS: ");
    intToStr(totalKills, tmp, 16);
    strAppend(buf, 64, tmp);

    sf::Text killText;
    killText.setFont(font);
    killText.setString(buf);
    killText.setCharacterSize(18);
    killText.setFillColor(sf::Color(200, 200, 200));
    int killLen = 0;
    while (buf[killLen])
    {
        killLen++;
    }
    float estKillWidth = killLen * 18.f * 0.45f;
    killText.setPosition((float)(screenW) - estKillWidth - 12.f, 32.f);
    window.draw(killText);

    if (multiplier >= 2)
    

{
        buf[0] = '\0';
        strAppend(buf, 64, "COMBO x");
        intToStr(multiplier, tmp, 16);
        strAppend(buf, 64, tmp);
        strAppend(buf, 64, "  (");
        intToStr(comboStreak, tmp, 16);
        strAppend(buf, 64, tmp);
        strAppend(buf, 64, " kills)");

        sf::Text comboText;
        comboText.setFont(font);
        comboText.setString(buf);
        comboText.setCharacterSize(20);
        comboText.setFillColor(sf::Color(255, 200, 0));  // golden
        int comboLen = 0;
        while (buf[comboLen])
        {
            comboLen++;
        }
        float estComboWidth = comboLen * 20.f * 0.45f;
        comboText.setPosition((float)(screenW) - estComboWidth - 12.f, 54.f);
        window.draw(comboText);
    }
}

// resetWave
void ScoreManager::resetWave()


{
    comboStreak = 0;
    multiplier = 1;
    comboTimer = 0;
    multiKillCount = 0;
    multiKillTimer = 0;
    damageTakenThisWave = false;
}

// basePointsFor
int ScoreManager::basePointsFor(int enemyType) const


{
    if (enemyType == Enemy::TYPE_REBEL)
    {
        return PTS_REBEL;
    }
    if (enemyType == Enemy::TYPE_SHIELDED)
    {
        return PTS_SHIELDED;
    }
    if (enemyType == Enemy::TYPE_BAZOOKA)
    {
        return PTS_BAZOOKA;
    }
    if (enemyType == Enemy::TYPE_GRENADE)
    {
        return PTS_GRENADE_SOL;
    }
    /*if (enemyType == Enemy::TYPE_ZOMBIE) return PTS_ZOMBIE;*/
    if (enemyType == Enemy::TYPE_MUMMY)
    {
        return PTS_MUMMY;
    }
   // if (enemyType == Enemy::TYPE_MARTIAN) return PTS_MARTIAN;
    return 100; // fallback
}

void ScoreManager::spawnPopup(float wx, float wy, int pts, const char* extra)


{
    for (int i = 0; i < MAX_POPUPS; i++)
    

{
        if (popups[i].active)
        {
            continue;
        }
        popups[i].x = wx;
        popups[i].y = wy - 20.f;   // spawn slightly above the enemy
        popups[i].velY = -55.f;
        popups[i].lifetime = popups[i].maxLife;
        popups[i].value = pts;
        popups[i].active = true;
        strCopy(popups[i].text, 40, extra);
        return;
    }
}

// recalcMultiplier
void ScoreManager::recalcMultiplier()


{
    if (comboStreak >= COMBO_FOR_X4)
    {
        multiplier = 4;
    }
    else if (comboStreak >= COMBO_FOR_X3) multiplier = 3;
    else if (comboStreak >= COMBO_FOR_X2) multiplier = 2;
    else multiplier = 1;
}

void ScoreManager::intToStr(int v, char* buf, int cap)


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

void ScoreManager::strAppend(char* dst, int cap, const char* src)


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

int ScoreManager::strLen(const char* s)


{
    int n = 0;
    while (s[n])
    {
        n++;
    }
    return n;
}

void ScoreManager::strCopy(char* dst, int cap, const char* src)


{
    int i = 0;
    while (src[i] && i < cap - 1) 

{

    dst[i] = src[i];
    i++;

}
    dst[i] = '\0';
}






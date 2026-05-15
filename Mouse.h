#pragma once
#include <SFML/Graphics.hpp>
#include "Soldier.h"
#include "Camera.h"
#include "Projectile.h"
#include <cmath>

class MouseControl {
public:
    // Header-only implementation to prevent unresolved external symbol errors during build
    static void handleContinuousFiring(Soldier* player, sf::RenderWindow& window, const Camera& cam, float dt) {
        if (!player || player->isDead()) return;

        static bool wasLeftDown = false;
        bool isLeftDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        if (isLeftDown) {
            // Detect exact accurate cursor pixel relative to application window dimensions
            sf::Vector2i mousePx = sf::Mouse::getPosition(window);
            
            // Map from localized window pixels back to exact infinite game world absolute floating coords!
            float clickX = (float)mousePx.x + cam.x;
            float clickY = (float)mousePx.y + cam.y;

            // Extract strict physics gun spawn locations from the current active player sprite structure
            float spawnX = player->getX() + (player->isFacingRight() ? 75.f : 15.f);
            float spawnY = player->getY() + 68.f;

            float dx = clickX - spawnX;
            float dy = clickY - spawnY;
            float angleRad = atan2f(dy, dx);

            // Orient character model to strictly align forward-facing facing vectors with the current firing vector 
            player->setFacingRight(dx >= 0);

            // Dynamically determine automatic rapid-fire vs static semi-auto single-shell behavior thresholds
            bool isAutomatic = (player->currentWeaponAnim == Soldier::WEAPON_HMG || player->currentWeaponAnim == Soldier::WEAPON_FLAME);
            
            // Edge-triggered for pistols, level-triggered for heavy payloads
            bool triggerShell = isAutomatic ? true : (!wasLeftDown);

            if (triggerShell) {
                int count = 0;
                Projectile** shots = player->shoot(angleRad, count);
                if (shots) {
                    delete[] shots; // Prevent low-level memory leaks
                }
            }
        }
        
        wasLeftDown = isLeftDown;
    }
};

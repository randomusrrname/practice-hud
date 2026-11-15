// Desbloqueos: iconos, colores, tiendas, niveles principales y
// Practice Music Sync. Todo es local (no toca el servidor).

#include "common.hpp"

#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/GameLevelManager.hpp>

#include <cstring>

using namespace geode::prelude;

// "Practice Music Sync" es el item GJItem 17.
constexpr int kPracticeMusicItem = 17;

class $modify(PhudGameManager, GameManager) {
    bool isColorUnlocked(int id, UnlockType type) {
        if (GameManager::isColorUnlocked(id, type)) return true;
        return phud::on("unlock-icons");
    }

    bool isIconUnlocked(int id, IconType type) {
        if (GameManager::isIconUnlocked(id, type)) return true;
        return phud::on("unlock-icons");
    }

    // Tiendas secretas: 11 Scratch, 20 Potbor, 34 Diamond Shopkeeper, 35 Mechanic.
    bool getUGV(char const* key) {
        if (GameManager::getUGV(key)) return true;
        if (!phud::on("unlock-shops")) return false;
        return std::strcmp(key, "11") == 0 || std::strcmp(key, "20") == 0
            || std::strcmp(key, "34") == 0 || std::strcmp(key, "35") == 0;
    }
};

class $modify(PhudStatsManager, GameStatsManager) {
    bool isItemUnlocked(UnlockType type, int id) {
        if (GameStatsManager::isItemUnlocked(type, id)) return true;

        if (type == UnlockType::GJItem) {
            if (id == kPracticeMusicItem && phud::on("practice-music")) return true;
            if (phud::on("unlock-icons") && id >= 18 && id <= 20) return true;
        }
        return false;
    }
};

class $modify(PhudLevelManager, GameLevelManager) {
    GJGameLevel* getMainLevel(int levelID, bool dontGetLevelString) {
        auto level = GameLevelManager::getMainLevel(levelID, dontGetLevelString);
        if (level && level->m_requiredCoins > 0 && phud::on("unlock-main-levels")) {
            level->m_requiredCoins = 0;
        }
        return level;
    }
};

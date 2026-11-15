// Herramientas de practica: musica en practica, practica automatica
// y quitar el efecto de muerte.

#include "common.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/CCTextInputNode.hpp>

using namespace geode::prelude;

namespace {
    constexpr int kPracticeMusicItem = 17;

    void applyPracticeMusicSetting() {
        auto mod = Mod::get();
        auto stats = GameStatsManager::sharedState();
        if (!stats) return;

        if (mod->getSettingValue<bool>("practice-music")) {
            stats->toggleEnableItem(UnlockType::GJItem, kPracticeMusicItem, true);
            mod->setSavedValue<bool>("forced-enable", true);
        } else if (mod->getSavedValue<bool>("forced-enable", false)) {
            // Solo lo apagamos si fuimos nosotros quienes lo prendimos.
            stats->toggleEnableItem(UnlockType::GJItem, kPracticeMusicItem, false);
            mod->setSavedValue<bool>("forced-enable", false);
        }
    }
}

class $modify(PhudPracticeLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool dontCreateObjects) {
        // Antes del init original: asi el juego lee ya el valor correcto.
        applyPracticeMusicSetting();

        if (!PlayLayer::init(level, dontCreateObjects)) return false;

        if (phud::on("auto-practice")) {
            this->togglePracticeMode(true);
        }
        return true;
    }
};

class $modify(PhudPlayerObject, PlayerObject) {
    void playDeathEffect() {
        if (phud::on("no-death-effect")) return;
        PlayerObject::playDeathEffect();
    }
};

// Bypass: sin limite de caracteres en los campos de texto.
class $modify(PhudTextInput, CCTextInputNode) {
    void updateLabel(std::string text) {
        if (phud::on("char-limit")) this->setMaxLabelLength(99999);
        CCTextInputNode::updateLabel(text);
    }
};

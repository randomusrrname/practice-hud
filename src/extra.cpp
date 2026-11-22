// Hacks extra: jump hack, retraso de reaparicion, efectos visuales
// (sin espejo, sin brillo, sin flash, sin fade de musica) y limite de checkpoints.

#include "common.hpp"

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

using namespace geode::prelude;

// -------------------------------------------------------------------
// Jump hack: permite saltar en el aire.
// -------------------------------------------------------------------
namespace {
    bool g_jumpPressed = false;
}

class $modify(PhudJumpLayer, GJBaseGameLayer) {
    void update(float dt) {
        bool hack = g_jumpPressed && phud::on("jump-hack") && PlayLayer::get();

        if (hack) {
            if (m_player1) m_player1->m_isOnGround = true;
            if (m_player2) m_player2->m_isOnGround = true;
        }

        GJBaseGameLayer::update(dt);

        if (hack) {
            if (m_player1) m_player1->m_isOnGround = true;
            if (m_player2) m_player2->m_isOnGround = true;
            g_jumpPressed = false;
        }
    }
};

class $modify(PhudJumpPlayer, PlayerObject) {
    bool pushButton(PlayerButton button) {
        if (phud::on("jump-hack") && PlayLayer::get()) {
            g_jumpPressed = true;
            phud::g_tripped = true; // cuenta como truco para el Modo Seguro
        }
        return PlayerObject::pushButton(button);
    }

    // Sin flash al reaparecer.
    void playSpawnEffect() {
        if (phud::on("no-respawn-flash")) return;
        PlayerObject::playSpawnEffect();
    }
};

// -------------------------------------------------------------------
// Efectos visuales y niveles
// -------------------------------------------------------------------
class $modify(PhudVisualBaseLayer, GJBaseGameLayer) {
    // Portales de espejo: no voltean la pantalla.
    void toggleFlipped(bool flip, bool noEffects) {
        if (phud::on("no-mirror")) return;
        GJBaseGameLayer::toggleFlipped(flip, noEffects);
    }
};

class $modify(PhudExtraLayer, PlayLayer) {
    // Sin brillo en los objetos.
    void addObject(GameObject* object) {
        if (object && phud::on("no-glow")) object->m_hasNoGlow = true;
        PlayLayer::addObject(object);
    }

    // Retraso de reaparicion configurable (0 = instantaneo).
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        if (!phud::on("respawn-delay-on")) return;
        // 0x10 es el tag de la secuencia de reaparicion del juego.
        if (auto sequence = this->getActionByTag(0x10)) {
            auto delay = static_cast<float>(phud::num("respawn-delay"));
            this->stopAction(sequence);
            auto newSequence = CCSequence::create(
                CCDelayTime::create(delay),
                CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
                nullptr
            );
            newSequence->setTag(0x10);
            this->runAction(newSequence);
        }
    }

    // Sin limite de checkpoints en practica.
    void storeCheckpoint(CheckpointObject* checkpoint) {
        if (!phud::on("checkpoint-limit") || !checkpoint) {
            PlayLayer::storeCheckpoint(checkpoint);
            return;
        }
        m_checkpointArray->addObject(checkpoint);
        this->addToSection(checkpoint->m_physicalCheckpointObject);
    }
};

// La musica no hace fade out al morir.
class $modify(PhudAudioEngine, FMODAudioEngine) {
    void fadeOutMusic(float duration, int channel) {
        if (phud::on("no-music-fadeout")) return;
        FMODAudioEngine::fadeOutMusic(duration, channel);
    }
};

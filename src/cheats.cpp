// Noclip, Speedhack y Modo Seguro.

#include "common.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <algorithm>

using namespace geode::prelude;

// -------------------------------------------------------------------
// Noclip + Modo Seguro (en PlayLayer)
// -------------------------------------------------------------------
class $modify(PhudCheatsLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        // El spike anti-cheat del juego siempre debe matar.
        if (object == m_anticheatSpike) {
            PlayLayer::destroyPlayer(player, object);
            return;
        }

        if (phud::on("noclip")) {
            bool p1 = phud::on("noclip-p1") && player == m_player1;
            bool p2 = phud::on("noclip-p2") && player == m_player2;
            if (p1 || p2) {
                // Nos salvamos de una muerte: este intento ya no es limpio.
                phud::g_tripped = true;
                return;
            }
        }

        // Muerte real: si el Modo Seguro esta activo, el juego la trata
        // como una prueba (no guarda mejor porcentaje, etc.).
        bool wasTest = m_isTestMode;
        if (phud::safeModeActive()) m_isTestMode = true;
        PlayLayer::destroyPlayer(player, object);
        m_isTestMode = wasTest;
    }

    void levelComplete() {
        // Con trucos, el nivel no se marca como completado de verdad.
        bool wasTest = m_isTestMode;
        if (phud::safeModeActive()) m_isTestMode = true;
        PlayLayer::levelComplete();
        m_isTestMode = wasTest;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        phud::g_tripped = false; // intento nuevo, empieza limpio
    }
};

// -------------------------------------------------------------------
// Speedhack: multiplica el tiempo que ve el juego.
// -------------------------------------------------------------------
namespace {
    // FPS bypass: sube (o restaura) el limite de cuadros por segundo.
    void applyFpsBypass() {
        static bool s_lastOn = false;
        static int s_lastTarget = -1;

        bool want = phud::on("fps-bypass");
        int target = std::clamp(static_cast<int>(phud::num("fps-target")), 30, 360);
        if (want == s_lastOn && (!want || target == s_lastTarget)) return;
        s_lastOn = want;
        s_lastTarget = target;

        auto gm = GameManager::sharedState();
        if (!gm) return;
        gm->setGameVariable(GameVar::UnlockFPS, want);
        if (want) gm->m_customFPSTarget = static_cast<float>(target);
        CCDirector::get()->setAnimationInterval(1.0 / (want ? target : 60));
    }
}

class $modify(PhudScheduler, CCScheduler) {
    void update(float dt) {
        // Cada medio segundo revisa si cambio el ajuste de FPS.
        static float s_fpsTimer = 1000.f;
        s_fpsTimer += dt;
        if (s_fpsTimer >= 0.5f) {
            s_fpsTimer = 0.f;
            applyFpsBypass();
        }

        if (phud::on("speedhack-on")) {
            auto speed = static_cast<float>(phud::num("speedhack"));
            if (speed > 0.f && speed != 1.f) {
                dt *= speed;
                if (PlayLayer::get()) phud::g_tripped = true;
            }
        }
        CCScheduler::update(dt);
    }
};

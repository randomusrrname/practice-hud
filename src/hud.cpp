// HUD: FPS, CPS, CPS maximo, clicks del intento, intento y porcentaje.

#include "common.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>
#include <string>

using namespace geode::prelude;

namespace {
    using Clock = std::chrono::steady_clock;

    // ---------------------------------------------------------------
    // Contador de clicks
    // ---------------------------------------------------------------
    struct ClickTracker {
        std::deque<Clock::time_point> recent; // clicks del ultimo segundo
        int total = 0;                        // clicks de este intento
        int maxCps = 0;                       // CPS mas alto de este intento

        void reset() {
            recent.clear();
            total = 0;
            maxCps = 0;
        }

        void prune(Clock::time_point now) {
            while (!recent.empty() && now - recent.front() > std::chrono::seconds(1)) {
                recent.pop_front();
            }
        }

        void add() {
            auto now = Clock::now();
            recent.push_back(now);
            ++total;
            prune(now);
            maxCps = std::max(maxCps, static_cast<int>(recent.size()));
        }

        int cps() {
            prune(Clock::now());
            return static_cast<int>(recent.size());
        }
    };

    ClickTracker g_clicks;
}

// -------------------------------------------------------------------
// Nodo del HUD
// -------------------------------------------------------------------
class HudNode : public CCNode {
protected:
    CCLabelBMFont* m_label = nullptr;
    std::string m_lastText;

    // FPS
    float m_fpsTime = 0.f;
    int m_fpsFrames = 0;
    int m_fps = 0;

    // Ajustes (se releen cada tanto, asi los cambios se ven sin reiniciar)
    float m_settingsTimer = 1000.f;
    bool m_showHud = true;
    bool m_showFps = true;
    bool m_showCps = true;
    bool m_showMaxCps = false;
    bool m_showClicks = true;
    bool m_showAttempt = true;
    bool m_showPercent = false;

    bool init() override {
        if (!CCNode::init()) return false;

        m_label = CCLabelBMFont::create("", "bigFont.fnt");
        if (!m_label) return false;
        this->addChild(m_label);

        this->refreshSettings();
        this->scheduleUpdate();
        return true;
    }

    void refreshSettings() {
        auto mod = Mod::get();
        m_showHud = mod->getSettingValue<bool>("show-hud");
        m_showFps = mod->getSettingValue<bool>("show-fps");
        m_showCps = mod->getSettingValue<bool>("show-cps");
        m_showMaxCps = mod->getSettingValue<bool>("show-max-cps");
        m_showClicks = mod->getSettingValue<bool>("show-clicks");
        m_showAttempt = mod->getSettingValue<bool>("show-attempt");
        m_showPercent = mod->getSettingValue<bool>("show-percent");

        auto position = mod->getSettingValue<std::string>("hud-position");
        bool top = position.rfind("Arriba", 0) == 0;
        bool left = position.find("izquierda") != std::string::npos;

        auto win = CCDirector::get()->getWinSize();
        constexpr float margin = 6.f;

        m_label->setAnchorPoint({left ? 0.f : 1.f, top ? 1.f : 0.f});
        m_label->setAlignment(left ? kCCTextAlignmentLeft : kCCTextAlignmentRight);
        m_label->setPosition({
            left ? margin : win.width - margin,
            top ? win.height - margin : margin
        });
        m_label->setScale(static_cast<float>(mod->getSettingValue<double>("hud-scale")));
        m_label->setOpacity(static_cast<GLubyte>(mod->getSettingValue<int64_t>("hud-opacity")));
    }

public:
    static HudNode* create() {
        auto ret = new HudNode();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void update(float dt) override {
        // FPS: frames contados en medio segundo.
        m_fpsTime += dt;
        ++m_fpsFrames;
        if (m_fpsTime >= 0.5f) {
            m_fps = static_cast<int>(std::lround(m_fpsFrames / m_fpsTime));
            m_fpsTime = 0.f;
            m_fpsFrames = 0;
        }

        m_settingsTimer += dt;
        if (m_settingsTimer >= 0.5f) {
            m_settingsTimer = 0.f;
            this->refreshSettings();
        }

        m_label->setVisible(m_showHud);
        if (!m_showHud) return;

        std::string text;
        if (m_showFps) text += fmt::format("FPS: {}\n", m_fps);
        if (m_showCps) text += fmt::format("CPS: {}\n", g_clicks.cps());
        if (m_showMaxCps) text += fmt::format("Max CPS: {}\n", g_clicks.maxCps);
        if (m_showClicks) text += fmt::format("Clicks: {}\n", g_clicks.total);

        if (auto pl = PlayLayer::get()) {
            if (m_showAttempt) text += fmt::format("Intento: {}\n", pl->m_attempts);
            if (m_showPercent) text += fmt::format("{}%\n", static_cast<int>(pl->getCurrentPercent()));
        }

        if (!text.empty() && text.back() == '\n') text.pop_back();

        // Solo tocamos el label si el texto cambio (ahorra rendimiento).
        if (text != m_lastText) {
            m_lastText = text;
            m_label->setString(text.c_str());
        }
    }
};

// -------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------

// Cuenta los clicks (boton de salto) mientras se juega un nivel.
class $modify(PhudClickLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        if (down && button == 1 && PlayLayer::get()) {
            bool twoPlayer = m_levelSettings && m_levelSettings->m_twoPlayerMode;
            // En modo 2 jugadores cuentan los dos; si no, solo el jugador 1
            // (asi un click no se cuenta doble en dual).
            if (isPlayer1 || twoPlayer) g_clicks.add();
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};

// Crea el HUD al empezar el nivel y cuenta los clicks por intento.
class $modify(PhudHudLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool dontCreateObjects) {
        if (!PlayLayer::init(level, dontCreateObjects)) return false;

        g_clicks.reset();

        if (m_uiLayer) {
            if (auto hud = HudNode::create()) {
                hud->setID("practice-hud"_spr);
                m_uiLayer->addChild(hud, 1000);
            }
        }
        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        g_clicks.reset(); // los clicks cuentan por intento
    }
};

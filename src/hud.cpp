// HUD por posiciones: cada estadistica va a una de 9 posiciones (o apagada),
// CPS y clicks se pueden unir, y hasta 3 imagenes PNG con transparencia.

#include "common.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <deque>
#include <string>

using namespace geode::prelude;

namespace {
    using Clock = std::chrono::steady_clock;

    struct ClickTracker {
        std::deque<Clock::time_point> recent;
        int total = 0;
        int maxCps = 0;

        void reset() { recent.clear(); total = 0; maxCps = 0; }

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

    // slot 1..9 -> punto de la pantalla y anclaje.
    CCPoint slotPoint(int slot, CCSize const& win) {
        int n = slot - 1;
        int col = n % 3;
        int row = n / 3;
        constexpr float m = 6.f;
        float x = col == 0 ? m : (col == 1 ? win.width / 2.f : win.width - m);
        float y = row == 0 ? win.height - m : (row == 1 ? win.height / 2.f : m);
        return CCPoint{x, y};
    }

    CCPoint slotAnchor(int slot) {
        int n = slot - 1;
        return CCPoint{(n % 3) * 0.5f, 1.f - (n / 3) * 0.5f};
    }

    // Indices de estadisticas.
    enum Stat { Fps, Cps, MaxCps, Clicks, Attempt, Percent, StatCount };
    constexpr std::array<char const*, StatCount> kStatKeys = {
        "slot-fps", "slot-cps", "slot-maxcps", "slot-clicks", "slot-attempt", "slot-percent"
    };
}

class HudNode : public CCNode {
protected:
    std::array<CCLabelBMFont*, 9> m_labels{};
    std::array<std::string, 9> m_lastText{};

    struct Img {
        CCSprite* sprite = nullptr;
        std::string file;
    };
    std::array<Img, 3> m_imgs{};

    float m_fpsTime = 0.f;
    int m_fpsFrames = 0;
    int m_fps = 0;

    float m_settingsTimer = 1000.f;
    bool m_showHud = true;
    bool m_merge = false;
    std::array<int, StatCount> m_slot{};
    std::array<std::string, StatCount> m_name{};

    bool init() override {
        if (!CCNode::init()) return false;

        for (int i = 0; i < 9; ++i) {
            auto label = CCLabelBMFont::create("", "bigFont.fnt");
            if (!label) return false;
            this->addChild(label);
            m_labels[i] = label;
        }

        this->refreshSettings();
        this->scheduleUpdate();
        return true;
    }

    void refreshImage(int i, CCSize const& win) {
        auto mod = Mod::get();
        auto k = [&](char const* s) { return fmt::format("img{}-{}", i + 1, s); };
        auto& im = m_imgs[i];

        bool enabled = mod->getSettingValue<bool>(k("enabled"));
        auto file = mod->getSettingValue<std::string>(k("file"));
        int slot = phud::slotIndex(mod->getSettingValue<std::string>(k("slot")));

        if (!enabled || file.empty() || slot == 0) {
            if (im.sprite) im.sprite->setVisible(false);
            return;
        }

        if (im.file != file) {
            if (im.sprite) {
                im.sprite->removeFromParentAndCleanup(true);
                im.sprite = nullptr;
            }
            im.file = file;
            auto path = phud::imagesDir() + "/" + file;
            auto tex = CCTextureCache::sharedTextureCache()->addImage(path.c_str(), false);
            if (tex) {
                im.sprite = CCSprite::createWithTexture(tex);
                if (im.sprite) this->addChild(im.sprite, 1);
            }
        }
        if (!im.sprite) return; // no se pudo cargar: no reintentamos hasta cambiar de archivo

        auto base = slotPoint(slot, win);
        im.sprite->setVisible(true);
        im.sprite->setAnchorPoint(slotAnchor(slot));
        im.sprite->setPosition({
            base.x + static_cast<float>(mod->getSettingValue<int64_t>(k("offset-x"))),
            base.y + static_cast<float>(mod->getSettingValue<int64_t>(k("offset-y")))
        });
        im.sprite->setScale(static_cast<float>(mod->getSettingValue<double>(k("scale"))));
        im.sprite->setOpacity(static_cast<GLubyte>(mod->getSettingValue<int64_t>(k("opacity"))));
    }

    void refreshSettings() {
        auto mod = Mod::get();
        auto win = CCDirector::get()->getWinSize();

        m_showHud = mod->getSettingValue<bool>("show-hud");
        m_merge = mod->getSettingValue<bool>("merge-cps-clicks");
        for (int s = 0; s < StatCount; ++s) {
            m_slot[s] = phud::slotIndex(mod->getSettingValue<std::string>(kStatKeys[s]));
        }
        m_name[Fps] = phud::tr("FPS");
        m_name[Cps] = phud::tr("CPS");
        m_name[MaxCps] = phud::tr("Max CPS");
        m_name[Clicks] = phud::tr("Clicks");
        m_name[Attempt] = phud::tr("Attempt");
        m_name[Percent] = phud::tr("Percent");

        float scale = static_cast<float>(mod->getSettingValue<double>("hud-scale"));
        auto opacity = static_cast<GLubyte>(mod->getSettingValue<int64_t>("hud-opacity"));

        for (int i = 0; i < 9; ++i) {
            int slot = i + 1;
            int col = i % 3;
            auto label = m_labels[i];
            label->setAnchorPoint(slotAnchor(slot));
            label->setAlignment(
                col == 0 ? kCCTextAlignmentLeft :
                (col == 1 ? kCCTextAlignmentCenter : kCCTextAlignmentRight)
            );
            label->setPosition(slotPoint(slot, win));
            label->setScale(scale);
            label->setOpacity(opacity);
        }

        for (int i = 0; i < 3; ++i) this->refreshImage(i, win);
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

        for (auto& im : m_imgs) {
            if (im.sprite && !m_showHud) im.sprite->setVisible(false);
        }
        if (!m_showHud) {
            for (auto label : m_labels) label->setVisible(false);
            return;
        }

        std::array<std::string, 10> text{}; // indice = slot (1..9)
        auto add = [&](int slot, std::string const& line) {
            if (slot <= 0) return;
            auto& t = text[slot];
            if (!t.empty()) t += '\n';
            t += line;
        };

        bool merged = m_merge && m_slot[Cps] > 0 && m_slot[Clicks] > 0;

        add(m_slot[Fps], fmt::format("{}: {}", m_name[Fps], m_fps));
        if (merged) {
            add(m_slot[Cps], fmt::format("{}: {} | {}: {}",
                m_name[Cps], g_clicks.cps(), m_name[Clicks], g_clicks.total));
        } else {
            add(m_slot[Cps], fmt::format("{}: {}", m_name[Cps], g_clicks.cps()));
        }
        add(m_slot[MaxCps], fmt::format("{}: {}", m_name[MaxCps], g_clicks.maxCps));
        if (!merged) {
            add(m_slot[Clicks], fmt::format("{}: {}", m_name[Clicks], g_clicks.total));
        }
        if (auto pl = PlayLayer::get()) {
            add(m_slot[Attempt], fmt::format("{}: {}", m_name[Attempt], pl->m_attempts));
            add(m_slot[Percent], fmt::format("{}%", static_cast<int>(pl->getCurrentPercent())));
        }

        for (int i = 0; i < 9; ++i) {
            auto const& t = text[i + 1];
            m_labels[i]->setVisible(!t.empty());
            if (t != m_lastText[i]) {
                m_lastText[i] = t;
                m_labels[i]->setString(t.c_str());
            }
        }
    }
};

// Cuenta los clicks (boton de salto) mientras se juega un nivel.
class $modify(PhudClickLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        if (down && button == 1 && PlayLayer::get()) {
            bool twoPlayer = m_levelSettings && m_levelSettings->m_twoPlayerMode;
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
        g_clicks.reset();
    }
};

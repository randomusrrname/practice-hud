// Menu de configuracion (GUI propio con FLAlertLayer + nodos de GD).
// Paginas con flechas; cada fila es un interruptor, un selector o un numero.

#include "common.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string_view>

using namespace geode::prelude;

namespace {
    enum class K { Bool, Num, Choice, File };
    using Opts = std::vector<char const*>;

    struct Row {
        K kind;
        char const* key;
        char const* label;
        double min = 0, max = 0, step = 0;
        bool isInt = false;
        Opts opts;
    };

    struct Page {
        char const* title;
        std::vector<Row> rows;
    };

    Row B(char const* k, char const* l) { return Row{K::Bool, k, l}; }
    Row N(char const* k, char const* l, double lo, double hi, double st, bool isInt = false) {
        return Row{K::Num, k, l, lo, hi, st, isInt};
    }
    Row C(char const* k, char const* l, Opts o) {
        return Row{K::Choice, k, l, 0, 0, 0, false, std::move(o)};
    }
    Row F(char const* k, char const* l) { return Row{K::File, k, l}; }

#define PHUD_IMG_PAGE(n, title) Page{title, { \
        B("img" #n "-enabled", "Enabled"), \
        F("img" #n "-file", "File"), \
        C("img" #n "-slot", "Position", slots), \
        N("img" #n "-scale", "Scale", 0.05, 3.0, 0.05), \
        N("img" #n "-opacity", "Opacity", 0, 255, 15, true), \
        N("img" #n "-offset-x", "Offset X", -300, 300, 5, true), \
        N("img" #n "-offset-y", "Offset Y", -300, 300, 5, true) }}

    std::vector<Page> const& pages() {
        static std::vector<Page> const p = [] {
            Opts slots(phud::kSlots.begin(), phud::kSlots.end());
            Opts langs = {"English", "Espa\xC3\xB1ol"};
            std::vector<Page> v;
            v.push_back(Page{"General", {
                C("language", "Language", langs),
                B("show-hud", "Show HUD"),
                B("ball-hide-ingame", "Hide ball while playing")
            }});
            v.push_back(Page{"Practice", {
                B("practice-music", "Practice music"),
                B("auto-practice", "Auto practice"),
                B("no-death-effect", "No death effect")
            }});
            v.push_back(Page{"Cheats", {
                B("safe-mode", "Safe mode"),
                B("noclip", "Noclip"),
                B("noclip-p1", "Noclip P1"),
                B("noclip-p2", "Noclip P2"),
                B("speedhack-on", "Speedhack"),
                N("speedhack", "Speed", 0.1, 5.0, 0.1)
            }});
            v.push_back(Page{"Hitboxes", {
                B("show-hitboxes", "Show hitboxes"),
                B("hitbox-solids", "Solids"),
                B("hitbox-hazards", "Hazards"),
                B("hitbox-others", "Others"),
                N("hitbox-fill", "Fill", 0.0, 1.0, 0.05)
            }});
            v.push_back(Page{"Bypass", {
                B("unlock-icons", "Unlock icons"),
                B("unlock-shops", "Unlock shops"),
                B("unlock-main-levels", "Unlock main levels"),
                B("char-limit", "No char limit")
            }});
            v.push_back(Page{"HUD", {
                C("slot-fps", "FPS", slots),
                C("slot-cps", "CPS", slots),
                C("slot-maxcps", "Max CPS", slots),
                C("slot-clicks", "Clicks", slots),
                C("slot-attempt", "Attempt", slots),
                C("slot-percent", "Percent", slots),
                B("merge-cps-clicks", "Merge CPS + Clicks")
            }});
            v.push_back(Page{"HUD Style", {
                N("hud-scale", "Text size", 0.2, 1.0, 0.05),
                N("hud-opacity", "Opacity", 30, 255, 15, true)
            }});
            v.push_back(PHUD_IMG_PAGE(1, "Image 1"));
            v.push_back(PHUD_IMG_PAGE(2, "Image 2"));
            v.push_back(PHUD_IMG_PAGE(3, "Image 3"));
            return v;
        }();
        return p;
    }

    std::string valueText(Row const& r) {
        auto mod = Mod::get();
        switch (r.kind) {
            case K::Num:
                if (r.isInt) return fmt::format("{}", mod->getSettingValue<int64_t>(r.key));
                return fmt::format("{:.2f}", mod->getSettingValue<double>(r.key));
            case K::Choice: {
                auto v = phud::str(r.key);
                if (v == "Espa\xC3\xB1ol") return "Espanol";
                return phud::tr(v);
            }
            case K::File: {
                auto v = phud::str(r.key);
                return v.empty() ? phud::tr("None") : v;
            }
            default: return "";
        }
    }
}

class PhudGui : public FLAlertLayer {
protected:
    static inline int s_page = 0;
    static inline PhudGui* s_open = nullptr;

    CCNode* m_content = nullptr;
    std::vector<CCLabelBMFont*> m_values;

    static constexpr float W = 360.f;
    static constexpr float H = 300.f;
    static constexpr float kRowH = 23.f;

    bool setup() {
        if (!FLAlertLayer::init(150)) return false;

        m_mainLayer = CCLayer::create();
        this->addChild(m_mainLayer);

        m_buttonMenu = CCMenu::create();
        m_buttonMenu->setPosition({0.f, 0.f});
        m_buttonMenu->setTouchPriority(-501);
        m_mainLayer->addChild(m_buttonMenu, 10);

        this->setTouchEnabled(true);
        this->setKeypadEnabled(true);

        auto win = CCDirector::get()->getWinSize();
        auto bg = extension::CCScale9Sprite::create("GJ_square01.png", {0.f, 0.f, 80.f, 80.f});
        bg->setContentSize({W, H});
        bg->setPosition({win.width / 2.f, win.height / 2.f});
        m_mainLayer->addChild(bg, 0);

        this->build();
        s_open = this;
        return true;
    }

    CCMenuItemSpriteExtra* arrow(bool right, int tag, SEL_MenuHandler cb, float scale) {
        CCNode* spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        if (spr) {
            spr->setScale(scale);
            static_cast<CCSprite*>(spr)->setFlipX(right);
        } else {
            spr = ButtonSprite::create(right ? ">" : "<");
        }
        auto btn = CCMenuItemSpriteExtra::create(spr, this, cb);
        btn->setTag(tag);
        return btn;
    }

    static void fit(CCLabelBMFont* lab) { lab->limitLabelWidth(82.f, 0.5f, 0.15f); }

    void build() {
        auto const& all = pages();
        int n = static_cast<int>(all.size());
        s_page = ((s_page % n) + n) % n;
        auto const& page = all[s_page];

        m_buttonMenu->removeAllChildrenWithCleanup(true);
        if (m_content) m_content->removeFromParentAndCleanup(true);
        m_content = CCNode::create();
        m_mainLayer->addChild(m_content, 2);
        m_values.assign(page.rows.size(), nullptr);

        auto win = CCDirector::get()->getWinSize();
        float cx = win.width / 2.f;
        float cy = win.height / 2.f;
        float left = cx - W / 2.f;
        float top = cy + H / 2.f;
        float bottom = cy - H / 2.f;

        // Titulo y paginas.
        auto title = CCLabelBMFont::create("Practice Suite", "goldFont.fnt");
        title->setScale(0.7f);
        title->setPosition({cx, top - 20.f});
        m_content->addChild(title);

        auto pageTitle = CCLabelBMFont::create(
            fmt::format("{}  ({}/{})", phud::tr(page.title), s_page + 1, n).c_str(), "bigFont.fnt");
        pageTitle->limitLabelWidth(150.f, 0.55f, 0.2f);
        pageTitle->setPosition({cx, top - 48.f});
        m_content->addChild(pageTitle);

        auto prev = this->arrow(false, 0, menu_selector(PhudGui::onPage), 0.6f);
        prev->setPosition({cx - 110.f, top - 48.f});
        m_buttonMenu->addChild(prev);
        auto next = this->arrow(true, 1, menu_selector(PhudGui::onPage), 0.6f);
        next->setPosition({cx + 110.f, top - 48.f});
        m_buttonMenu->addChild(next);

        // Filas.
        float y0 = top - 76.f;
        for (size_t i = 0; i < page.rows.size(); ++i) {
            auto const& r = page.rows[i];
            float y = y0 - static_cast<float>(i) * kRowH;

            if (i % 2 == 0) {
                if (auto stripe = extension::CCScale9Sprite::create("square02b_001.png", {0.f, 0.f, 80.f, 80.f})) {
                    stripe->setColor({0, 0, 0});
                    stripe->setOpacity(70);
                    stripe->setContentSize({W - 24.f, kRowH - 2.f});
                    stripe->setPosition({cx, y});
                    m_content->addChild(stripe, -1);
                }
            }

            auto lbl = CCLabelBMFont::create(phud::tr(r.label).c_str(), "bigFont.fnt");
            lbl->setAnchorPoint({0.f, 0.5f});
            lbl->setPosition({left + 22.f, y});
            lbl->limitLabelWidth(170.f, 0.45f, 0.15f);
            m_content->addChild(lbl);

            if (r.kind == K::Bool) {
                auto t = CCMenuItemToggler::createWithStandardSprites(
                    this, menu_selector(PhudGui::onBool), 0.6f);
                t->setTag(static_cast<int>(i));
                t->setPosition({cx + 100.f, y});
                t->toggle(phud::on(r.key));
                m_buttonMenu->addChild(t);
            } else {
                auto lb = this->arrow(false, static_cast<int>(i) * 2, menu_selector(PhudGui::onStep), 0.45f);
                lb->setPosition({cx + 45.f, y});
                m_buttonMenu->addChild(lb);
                auto rb = this->arrow(true, static_cast<int>(i) * 2 + 1, menu_selector(PhudGui::onStep), 0.45f);
                rb->setPosition({cx + 155.f, y});
                m_buttonMenu->addChild(rb);

                auto val = CCLabelBMFont::create(valueText(r).c_str(), "bigFont.fnt");
                val->setPosition({cx + 100.f, y});
                fit(val);
                m_content->addChild(val);
                m_values[i] = val;
            }
        }

        // Ayuda en las paginas de imagenes.
        if (std::string_view(page.title).starts_with("Image")) {
            auto files = phud::listImages();
            auto hint = CCLabelBMFont::create(
                phud::tr(files.empty() ? "No PNGs found. Put PNG files in:" : "Put PNG files in:").c_str(),
                "chatFont.fnt");
            hint->limitLabelWidth(330.f, 0.6f, 0.2f);
            hint->setPosition({cx, bottom + 64.f});
            m_content->addChild(hint);
            auto path = CCLabelBMFont::create(phud::imagesDir().c_str(), "chatFont.fnt");
            path->limitLabelWidth(335.f, 0.5f, 0.1f);
            path->setPosition({cx, bottom + 52.f});
            path->setOpacity(190);
            m_content->addChild(path);
        }

        // Botones de abajo.
        auto closeBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(phud::tr("Close").c_str()), this, menu_selector(PhudGui::onClose));
        closeBtn->setPosition({cx + 70.f, bottom + 26.f});
        m_buttonMenu->addChild(closeBtn);

        auto resetBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(phud::tr("Reset ball").c_str()), this, menu_selector(PhudGui::onResetBall));
        resetBtn->setPosition({cx - 70.f, bottom + 26.f});
        m_buttonMenu->addChild(resetBtn);
    }

    void onRebuild(float) { this->build(); }

    // Reconstruir dentro de un callback de boton puede liberar el boton
    // que se esta usando: se hace un instante despues.
    void deferRebuild() {
        this->scheduleOnce(schedule_selector(PhudGui::onRebuild), 0.f);
    }

    void onPage(CCObject* sender) {
        int tag = static_cast<CCNode*>(sender)->getTag();
        s_page += (tag == 0) ? -1 : 1;
        this->deferRebuild();
    }

    void onBool(CCObject* sender) {
        auto t = static_cast<CCMenuItemToggler*>(sender);
        auto const& r = pages()[s_page].rows[t->getTag()];
        (void)Mod::get()->setSettingValue<bool>(r.key, !t->isToggled());
    }

    void onStep(CCObject* sender) {
        int tag = static_cast<CCNode*>(sender)->getTag();
        size_t i = static_cast<size_t>(tag / 2);
        int dir = (tag % 2) ? 1 : -1;
        auto const& r = pages()[s_page].rows[i];
        auto mod = Mod::get();

        if (r.kind == K::Num) {
            double cur = r.isInt
                ? static_cast<double>(mod->getSettingValue<int64_t>(r.key))
                : mod->getSettingValue<double>(r.key);
            double v = std::round((cur + dir * r.step) / r.step) * r.step;
            v = std::clamp(v, r.min, r.max);
            if (r.isInt) (void)mod->setSettingValue<int64_t>(r.key, static_cast<int64_t>(std::llround(v)));
            else (void)mod->setSettingValue<double>(r.key, v);
        } else if (r.kind == K::Choice) {
            auto cur = phud::str(r.key);
            int count = static_cast<int>(r.opts.size());
            int idx = 0;
            for (int j = 0; j < count; ++j) {
                if (cur == r.opts[j]) idx = j;
            }
            idx = (idx + dir + count) % count;
            (void)mod->setSettingValue<std::string>(r.key, std::string(r.opts[idx]));
            if (std::strcmp(r.key, "language") == 0) {
                this->deferRebuild(); // cambia todos los textos
                return;
            }
        } else if (r.kind == K::File) {
            std::vector<std::string> files = {""};
            for (auto& f : phud::listImages()) files.push_back(f);
            auto cur = phud::str(r.key);
            int count = static_cast<int>(files.size());
            int idx = 0;
            for (int j = 0; j < count; ++j) {
                if (cur == files[j]) idx = j;
            }
            idx = (idx + dir + count) % count;
            (void)mod->setSettingValue<std::string>(r.key, files[idx]);
        }

        if (i < m_values.size() && m_values[i]) {
            m_values[i]->setString(valueText(r).c_str());
            fit(m_values[i]);
        }
    }

    void onResetBall(CCObject*) { phud::resetBall(); }

    void onClose(CCObject*) {
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
        this->removeFromParentAndCleanup(true);
    }

    void keyBackClicked() override { this->onClose(nullptr); }

    void registerWithTouchDispatcher() override {
        CCDirector::get()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
    }

    bool ccTouchBegan(CCTouch*, CCEvent*) override { return true; }

public:
    ~PhudGui() override {
        if (s_open == this) s_open = nullptr;
    }

    static PhudGui* create() {
        auto ret = new PhudGui();
        if (ret->setup()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    static bool isOpen() { return s_open != nullptr; }
};

namespace phud {
    void openGui() {
        if (PhudGui::isOpen()) return;
        if (auto gui = PhudGui::create()) gui->show();
    }
}

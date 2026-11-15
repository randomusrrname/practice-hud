// Menu de configuracion (GUI propio con FLAlertLayer + nodos de GD).
// Paginas con flechas; cada fila es un interruptor, un selector o un numero.

#include "common.hpp"

#include <algorithm>
#include <algorithm>
#include <array>
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
                B("merge-cps-clicks", "Merge CPS + Clicks"),
                N("hud-scale", "Text size", 0.2, 1.0, 0.05),
                N("hud-opacity", "Text opacity", 30, 255, 15, true)
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

    static constexpr float W = 440.f;
    static constexpr float H = 282.f;
    static constexpr float kRowH = 24.f;

    // Paleta (estilo Eclipse).
    static constexpr ccColor3B kBorder{20, 23, 42};
    static constexpr ccColor3B kPanel{37, 42, 61};
    static constexpr ccColor3B kInner{30, 34, 54};
    static constexpr ccColor3B kTab{53, 58, 85};
    static constexpr ccColor3B kField{47, 52, 80};
    static constexpr ccColor3B kLight{230, 232, 242};
    static constexpr ccColor3B kAccent{108, 140, 255};
    static constexpr ccColor3B kDark{32, 36, 58};

    static extension::CCScale9Sprite* box(float w, float h, ccColor3B c, GLubyte op = 255) {
        auto s = extension::CCScale9Sprite::create("square02b_001.png", {0.f, 0.f, 80.f, 80.f});
        s->setContentSize({w, h});
        s->setColor(c);
        s->setOpacity(op);
        return s;
    }

    static CCLabelBMFont* text(std::string const& str, ccColor3B c, float maxW, float scale = 0.7f) {
        auto l = CCLabelBMFont::create(str.c_str(), "chatFont.fnt");
        l->setColor(c);
        l->limitLabelWidth(maxW, scale, 0.25f);
        return l;
    }

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
        auto border = box(W, H, kBorder);
        border->setPosition({win.width / 2.f, win.height / 2.f});
        m_mainLayer->addChild(border, 0);
        auto panel = box(W - 4.f, H - 4.f, kPanel);
        panel->setPosition({win.width / 2.f, win.height / 2.f});
        m_mainLayer->addChild(panel, 1);

        this->build();
        s_open = this;
        return true;
    }

    // Boton con fondo redondeado y texto centrado.
    CCMenuItemSpriteExtra* labelButton(std::string const& str, float w, float h, ccColor3B bg,
                                       ccColor3B fg, SEL_MenuHandler cb) {
        auto b = box(w, h, bg);
        auto l = text(str, fg, w - 12.f, 0.7f);
        l->setPosition({w / 2.f, h / 2.f});
        b->addChild(l);
        auto btn = CCMenuItemSpriteExtra::create(b, this, cb);
        btn->m_scaleMultiplier = 1.04f;
        return btn;
    }

    // Flecha triangular (izquierda o derecha) dentro de un area tocable.
    CCMenuItemSpriteExtra* arrow(bool right, int tag) {
        auto bg = box(26.f, 20.f, kField, 0);
        auto d = CCDrawNode::create();
        d->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        d->m_bUseArea = false;
        float cx = 13.f, cy = 10.f;
        std::array<CCPoint, 3> tri = right
            ? std::array<CCPoint, 3>{CCPoint{cx - 3.f, cy + 5.f}, CCPoint{cx - 3.f, cy - 5.f}, CCPoint{cx + 4.f, cy}}
            : std::array<CCPoint, 3>{CCPoint{cx + 3.f, cy + 5.f}, CCPoint{cx + 3.f, cy - 5.f}, CCPoint{cx - 4.f, cy}};
        d->drawPolygon(tri.data(), 3, {0.9f, 0.91f, 0.95f, 1.f}, 0.f, {0.f, 0.f, 0.f, 0.f});
        bg->addChild(d);
        auto btn = CCMenuItemSpriteExtra::create(bg, this, menu_selector(PhudGui::onStep));
        btn->m_scaleMultiplier = 1.2f;
        btn->setTag(tag);
        return btn;
    }

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
        float right = cx + W / 2.f;
        float top = cy + H / 2.f;
        float bottom = cy - H / 2.f;

        // ---- Barra lateral ----
        float sideX = left + 10.f + 56.f;
        auto title = text("Practice Suite", kAccent, 108.f, 0.8f);
        title->setPosition({sideX, top - 20.f});
        m_content->addChild(title);

        for (int i = 0; i < n; ++i) {
            bool sel = (i == s_page);
            auto btn = this->labelButton(phud::tr(all[i].title), 112.f, 22.f,
                sel ? kLight : kTab, sel ? kDark : kLight, menu_selector(PhudGui::onTab));
            btn->m_scaleMultiplier = 1.02f;
            btn->setTag(i);
            btn->setPosition({sideX, top - 46.f - static_cast<float>(i) * 25.f});
            m_buttonMenu->addChild(btn);
        }

        // ---- Panel de contenido ----
        float cLeft = left + 130.f;
        float cRight = right - 10.f;
        float cW = cRight - cLeft;
        auto inner = box(cW, H - 20.f, kInner);
        inner->setPosition({cLeft + cW / 2.f, cy});
        m_content->addChild(inner, -1);

        float y0 = top - 30.f;
        for (size_t i = 0; i < page.rows.size(); ++i) {
            auto const& r = page.rows[i];
            float y = y0 - static_cast<float>(i) * kRowH;
            auto label = phud::tr(r.label);

            if (r.kind == K::Bool) {
                auto off = box(18.f, 18.f, kTab);
                auto on = box(18.f, 18.f, kLight);
                auto dot = box(10.f, 10.f, kAccent);
                dot->setPosition({9.f, 9.f});
                on->addChild(dot);
                auto t = CCMenuItemToggler::create(off, on, this, menu_selector(PhudGui::onBool));
                t->setTag(static_cast<int>(i));
                t->setPosition({cLeft + 22.f, y});
                t->toggle(phud::on(r.key));
                m_buttonMenu->addChild(t);

                auto l = text(label, kLight, cW - 54.f);
                l->setAnchorPoint({0.f, 0.5f});
                l->setPosition({cLeft + 38.f, y});
                m_content->addChild(l);
            } else {
                auto l = text(label, kLight, cW - 160.f);
                l->setAnchorPoint({0.f, 0.5f});
                l->setPosition({cLeft + 14.f, y});
                m_content->addChild(l);

                float fw = 128.f;
                float fx = cRight - 10.f - fw / 2.f;
                auto field = box(fw, 20.f, kField);
                field->setPosition({fx, y});
                m_content->addChild(field);

                auto lb = this->arrow(false, static_cast<int>(i) * 2);
                lb->setPosition({fx - fw / 2.f + 13.f, y});
                m_buttonMenu->addChild(lb);
                auto rb = this->arrow(true, static_cast<int>(i) * 2 + 1);
                rb->setPosition({fx + fw / 2.f - 13.f, y});
                m_buttonMenu->addChild(rb);

                auto val = text(valueText(r), kLight, fw - 56.f, 0.65f);
                val->setPosition({fx, y});
                m_content->addChild(val);
                m_values[i] = val;
            }
        }

        // ---- Ayuda en las paginas de imagenes ----
        if (std::string_view(page.title).starts_with("Image")) {
            auto files = phud::listImages();
            auto hint = text(phud::tr(files.empty() ? "No PNGs found. Put PNG files in:" : "Put PNG files in:"),
                kLight, cW - 20.f, 0.6f);
            hint->setPosition({cLeft + cW / 2.f, bottom + 62.f});
            m_content->addChild(hint);
            auto path = text(phud::imagesDir(), {160, 166, 190}, cW - 20.f, 0.5f);
            path->setPosition({cLeft + cW / 2.f, bottom + 49.f});
            m_content->addChild(path);
        }

        // ---- Botones de abajo (ancho fijo: el texto en espanol no se pisa) ----
        float by = bottom + 28.f;
        float bw = (cW - 30.f) / 2.f;
        auto reset = this->labelButton(phud::tr("Reset ball"), bw, 24.f, kTab, kLight,
            menu_selector(PhudGui::onResetBall));
        reset->setPosition({cLeft + 10.f + bw / 2.f, by});
        m_buttonMenu->addChild(reset);
        auto close = this->labelButton(phud::tr("Close"), bw, 24.f, kTab, kLight,
            menu_selector(PhudGui::onClose));
        close->setPosition({cRight - 10.f - bw / 2.f, by});
        m_buttonMenu->addChild(close);
    }

    void onRebuild(float) { this->build(); }

    // Reconstruir dentro de un callback de boton puede liberar el boton
    // que se esta usando: se hace un instante despues.
    void deferRebuild() {
        this->scheduleOnce(schedule_selector(PhudGui::onRebuild), 0.f);
    }

    void onTab(CCObject* sender) {
        s_page = static_cast<CCNode*>(sender)->getTag();
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
            m_values[i]->limitLabelWidth(72.f, 0.65f, 0.25f);
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

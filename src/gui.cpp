// Menu de configuracion (GUI propio con FLAlertLayer + nodos de GD).
// v3.4: estilo moderno (esquinas redondeadas), secciones con descripcion,
// interruptores animados, color de acento configurable y animacion al
// abrir / cerrar / cambiar de pestana.

#include "common.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstring>
#include <string>
#include <string_view>

using namespace geode::prelude;

namespace {
    enum class K { Header, Bool, Num, Choice, File, Action };
    using Opts = std::vector<char const*>;

    struct Row {
        K kind;
        char const* key;
        char const* label;
        double min = 0, max = 0, step = 0;
        bool isInt = false;
        Opts opts;
        char const* desc = nullptr; // texto pequeno bajo el nombre (opcional)
    };

    struct Page {
        char const* title;
        std::vector<Row> rows;
    };

    Row Sec(char const* l) { return Row{K::Header, "", l}; }
    Row B(char const* k, char const* l, char const* d = nullptr) {
        Row r{K::Bool, k, l};
        r.desc = d;
        return r;
    }
    Row N(char const* k, char const* l, double lo, double hi, double st, bool isInt = false,
          char const* d = nullptr) {
        Row r{K::Num, k, l, lo, hi, st, isInt};
        r.desc = d;
        return r;
    }
    Row C(char const* k, char const* l, Opts o, char const* d = nullptr) {
        Row r{K::Choice, k, l, 0, 0, 0, false, std::move(o)};
        r.desc = d;
        return r;
    }
    Row F(char const* k, char const* l) { return Row{K::File, k, l}; }
    Row A(char const* l, char const* d = nullptr) {
        Row r{K::Action, "", l};
        r.desc = d;
        return r;
    }

#define PHUD_IMG_PAGE(n, title) Page{title, { \
        Sec("Image"), \
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
            Opts accents = {"Blue", "Purple", "Pink", "Red", "Orange", "Green", "Cyan"};
            std::vector<Page> v;
            v.push_back(Page{"General", {
                Sec("Interface"),
                C("language", "Language", langs, "Menu and HUD language"),
                C("accent", "Accent color", accents, "Highlight color of this menu"),
                Sec("Overlay"),
                B("show-hud", "Show HUD", "Stats on screen while playing"),
                B("ball-hide-ingame", "Hide ball while playing", "Reappears when you pause"),
                A("Floating ball", "Move it back to the default spot")
            }});
            v.push_back(Page{"Practice", {
                Sec("Practice mode"),
                B("practice-music", "Practice music", "Level music plays in practice mode"),
                B("auto-practice", "Auto practice", "Enters practice as the level starts"),
                B("no-death-effect", "No death effect", "Removes the death explosion"),
                B("respawn-delay-on", "Respawn delay", "Change the time before you respawn"),
                N("respawn-delay", "Delay (seconds)", 0.0, 2.0, 0.05)
            }});
            v.push_back(Page{"Cheats", {
                Sec("Safety"),
                B("safe-mode", "Safe mode", "Attempts with cheats save no progress"),
                Sec("Gameplay"),
                B("noclip", "Noclip", "Pass through obstacles"),
                B("noclip-p1", "Noclip P1"),
                B("noclip-p2", "Noclip P2"),
                B("speedhack-on", "Speedhack", "Changes the game speed"),
                N("speedhack", "Speed", 0.1, 5.0, 0.1),
                B("jump-hack", "Jump hack", "Jump in mid-air")
            }});
            v.push_back(Page{"Visual", {
                Sec("Effects"),
                B("no-mirror", "No mirror portal", "Mirror portals do not flip the screen"),
                B("no-glow", "No glow", "Removes the glow from objects"),
                B("no-respawn-flash", "No respawn flash", "Removes the flash when you respawn"),
                B("no-music-fadeout", "No music fade out", "Music keeps playing when you die"),
                Sec("Performance"),
                B("fps-bypass", "FPS bypass", "Raises the frame cap (limited by your screen)"),
                N("fps-target", "FPS target", 30, 360, 10, true)
            }});
            v.push_back(Page{"Hitboxes", {
                Sec("Hitboxes"),
                B("show-hitboxes", "Show hitboxes", "Draws the collision boxes"),
                Sec("Types"),
                B("hitbox-solids", "Solids"),
                B("hitbox-hazards", "Hazards"),
                B("hitbox-others", "Others"),
                N("hitbox-fill", "Fill", 0.0, 1.0, 0.05)
            }});
            v.push_back(Page{"Bypass", {
                Sec("Local only"),
                B("unlock-icons", "Unlock icons", "All icons, colors and trails"),
                B("unlock-shops", "Unlock shops", "Opens the secret shops"),
                B("unlock-main-levels", "Unlock main levels", "No coin requirement"),
                B("unlock-paths", "Unlock paths"),
                B("unlock-music-customizer", "Unlock music customizer"),
                B("char-limit", "No char limit", "No limit in text fields"),
                B("checkpoint-limit", "No checkpoint limit", "Unlimited checkpoints in practice")
            }});
            v.push_back(Page{"HUD", {
                Sec("Position"),
                C("slot-fps", "FPS", slots),
                C("slot-cps", "CPS", slots),
                C("slot-maxcps", "Max CPS", slots),
                C("slot-clicks", "Clicks", slots),
                C("slot-attempt", "Attempt", slots),
                C("slot-percent", "Percent", slots),
                B("merge-cps-clicks", "Merge CPS + Clicks"),
                Sec("Style"),
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

    ccColor3B accentFor(std::string const& n) {
        if (n == "Purple") return ccColor3B{170, 124, 255};
        if (n == "Pink") return ccColor3B{255, 124, 184};
        if (n == "Red") return ccColor3B{255, 104, 108};
        if (n == "Orange") return ccColor3B{255, 164, 74};
        if (n == "Green") return ccColor3B{92, 214, 140};
        if (n == "Cyan") return ccColor3B{72, 204, 232};
        return ccColor3B{108, 140, 255}; // Blue
    }

    std::string upper(std::string s) {
        for (auto& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        return s;
    }

    // ---- Dibujo: rectangulos redondeados con CCDrawNode ----

    constexpr float kPi = 3.14159265f;

    ccColor4F col4(ccColor3B c, GLubyte a = 255) {
        return ccColor4F{c.r / 255.f, c.g / 255.f, c.b / 255.f, a / 255.f};
    }

    std::vector<CCPoint> roundedPts(float w, float h, float r) {
        r = std::min(r, std::min(w, h) / 2.f);
        constexpr int steps = 7;
        float cx[4] = {w - r, r, r, w - r};
        float cy[4] = {h - r, h - r, r, r};
        std::vector<CCPoint> p;
        p.reserve(4 * (steps + 1));
        for (int c = 0; c < 4; ++c) {
            for (int i = 0; i <= steps; ++i) {
                float a = (90.f * static_cast<float>(c) + 90.f * static_cast<float>(i) / steps) * kPi / 180.f;
                p.push_back(CCPoint{cx[c] + r * std::cos(a), cy[c] + r * std::sin(a)});
            }
        }
        return p;
    }

    // Rectangulo redondeado centrado (ancla 0.5, 0.5) de tamano exacto w x h.
    CCDrawNode* rr(float w, float h, float r, ccColor3B c, GLubyte a = 255) {
        auto d = CCDrawNode::create();
        d->m_bUseArea = false;
        d->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        d->setContentSize({w, h});
        d->setAnchorPoint({0.5f, 0.5f});
        auto pts = roundedPts(w, h, r);
        d->drawPolygon(pts.data(), static_cast<unsigned int>(pts.size()), col4(c, a), 0.f,
                       ccColor4F{0.f, 0.f, 0.f, 0.f});
        return d;
    }

    // Rectangulo plano (se usa como area tocable invisible, opacidad 0).
    CCLayerColor* flat(float w, float h, ccColor3B c, GLubyte op = 255) {
        auto l = CCLayerColor::create(ccColor4B{c.r, c.g, c.b, op}, w, h);
        l->ignoreAnchorPointForPosition(false);
        l->setAnchorPoint({0.5f, 0.5f});
        return l;
    }

    // Triangulo pequeno (flecha) dibujado alrededor de (0, 0).
    CCDrawNode* tri(bool right, ccColor3B c) {
        auto d = CCDrawNode::create();
        d->m_bUseArea = false;
        d->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        std::array<CCPoint, 3> t = right
            ? std::array<CCPoint, 3>{CCPoint{-2.5f, 4.f}, CCPoint{-2.5f, -4.f}, CCPoint{3.5f, 0.f}}
            : std::array<CCPoint, 3>{CCPoint{2.5f, 4.f}, CCPoint{2.5f, -4.f}, CCPoint{-3.5f, 0.f}};
        d->drawPolygon(t.data(), 3, col4(c), 0.f, ccColor4F{0.f, 0.f, 0.f, 0.f});
        return d;
    }

    void addLine(CCDrawNode* d, CCPoint a, CCPoint b, float thick, ccColor3B c) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float len = std::max(0.001f, std::hypot(dx, dy));
        float nx = -dy / len * thick / 2.f;
        float ny = dx / len * thick / 2.f;
        std::array<CCPoint, 4> q = {
            CCPoint{a.x + nx, a.y + ny}, CCPoint{a.x - nx, a.y - ny},
            CCPoint{b.x - nx, b.y - ny}, CCPoint{b.x + nx, b.y + ny}
        };
        d->drawPolygon(q.data(), 4, col4(c), 0.f, ccColor4F{0.f, 0.f, 0.f, 0.f});
    }

    CCDrawNode* xMark(ccColor3B c) {
        auto d = CCDrawNode::create();
        d->m_bUseArea = false;
        d->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        addLine(d, {-4.f, -4.f}, {4.f, 4.f}, 1.8f, c);
        addLine(d, {-4.f, 4.f}, {4.f, -4.f}, 1.8f, c);
        return d;
    }

    CCLabelBMFont* text(std::string const& str, ccColor3B c, float maxW, float scale = 0.7f) {
        auto l = CCLabelBMFont::create(str.c_str(), "chatFont.fnt");
        l->setColor(c);
        l->limitLabelWidth(maxW, scale, 0.25f);
        return l;
    }

    // Si esta version de Geode tiene m_noElasticity, apagamos la animacion
    // elastica nativa para usar la nuestra. Si no existe, no hace nada.
    template <class T>
    void noElastic(T* t) {
        if constexpr (requires { t->m_noElasticity = true; }) t->m_noElasticity = true;
    }
}

class PhudGui : public FLAlertLayer {
protected:
    static inline int s_page = 0;
    static inline PhudGui* s_open = nullptr;

    struct Sw {
        CCNode* on = nullptr;
        CCNode* knob = nullptr;
        CCPoint at;
    };

    CCNode* m_root = nullptr;    // centro de la pantalla; aqui se anima la escala
    CCNode* m_chrome = nullptr;  // barra lateral + cabecera (se reconstruye)
    CCNode* m_pane = nullptr;    // filas de la pagina (se reconstruye / se desliza)
    CCMenu* m_paneMenu = nullptr;
    std::vector<CCLabelBMFont*> m_values;
    std::vector<Sw> m_sw;
    ccColor3B m_accent{108, 140, 255};
    bool m_closing = false;
    bool m_slideNext = false;

    // Medidas (el origen es el centro de la pantalla).
    static constexpr float kW = 470.f;
    static constexpr float kH = 304.f;
    static constexpr float kSideW = 118.f;
    static constexpr float kSideCx = -kW / 2.f + 8.f + kSideW / 2.f;
    static constexpr float kPanelL = -kW / 2.f + 8.f + kSideW + 8.f;
    static constexpr float kPanelR = kW / 2.f - 8.f;
    static constexpr float kPanelW = kPanelR - kPanelL;
    static constexpr float kPanelCx = (kPanelL + kPanelR) / 2.f;

    // Paleta.
    static constexpr ccColor3B kLine{50, 56, 84};
    static constexpr ccColor3B kBg{26, 29, 44};
    static constexpr ccColor3B kSide{20, 22, 34};
    static constexpr ccColor3B kPanel{31, 35, 53};
    static constexpr ccColor3B kRow{41, 46, 69};
    static constexpr ccColor3B kField{55, 61, 92};
    static constexpr ccColor3B kTab{32, 36, 56};
    static constexpr ccColor3B kText{232, 234, 244};
    static constexpr ccColor3B kSub{138, 145, 175};
    static constexpr ccColor3B kOff{70, 76, 108};
    static constexpr ccColor3B kKnob{236, 238, 248};
    static constexpr ccColor3B kDark{18, 20, 32};

    bool setup() {
        if (!FLAlertLayer::init(150)) return false;
        noElastic(this);

        m_mainLayer = CCLayer::create();
        this->addChild(m_mainLayer);

        auto win = CCDirector::get()->getWinSize();
        m_root = CCNode::create();
        m_root->setPosition({win.width / 2.f, win.height / 2.f});
        m_mainLayer->addChild(m_root);

        m_buttonMenu = CCMenu::create();
        m_buttonMenu->setPosition({0.f, 0.f});
        m_buttonMenu->setTouchPriority(-501);
        m_root->addChild(m_buttonMenu, 10);

        this->setTouchEnabled(true);
        this->setKeypadEnabled(true);

        // Fondo fijo: sombra, borde, tarjeta, barra lateral y panel.
        auto shadow = rr(kW + 8.f, kH + 8.f, 18.f, ccColor3B{0, 0, 0}, 90);
        shadow->setPosition({0.f, -3.f});
        m_root->addChild(shadow, 0);
        m_root->addChild(rr(kW, kH, 14.f, kLine), 0);
        m_root->addChild(rr(kW - 3.f, kH - 3.f, 13.f, kBg), 0);
        auto side = rr(kSideW, kH - 16.f, 10.f, kSide);
        side->setPosition({kSideCx, 0.f});
        m_root->addChild(side, 1);
        auto panel = rr(kPanelW, kH - 16.f, 10.f, kPanel);
        panel->setPosition({kPanelCx, 0.f});
        m_root->addChild(panel, 1);

        this->build(false);

        // Animacion de apertura.
        m_root->setScale(0.9f);
        m_root->runAction(CCEaseBackOut::create(CCScaleTo::create(0.22f, 1.f)));

        s_open = this;
        return true;
    }

    // Boton con fondo redondeado y texto centrado.
    CCMenuItemSpriteExtra* labelButton(std::string const& str, float w, float h, ccColor3B bg,
                                       ccColor3B fg, SEL_MenuHandler cb) {
        auto b = rr(w, h, std::min(h / 2.f, 9.f), bg);
        auto l = text(str, fg, w - 12.f, 0.7f);
        l->setPosition({w / 2.f, h / 2.f});
        b->addChild(l);
        auto btn = CCMenuItemSpriteExtra::create(b, this, cb);
        btn->m_scaleMultiplier = 1.04f;
        return btn;
    }

    // Area tocable invisible.
    CCMenuItemSpriteExtra* hitArea(float w, float h, int tag, SEL_MenuHandler cb) {
        auto n = flat(w, h, kRow, 0);
        auto btn = CCMenuItemSpriteExtra::create(n, this, cb);
        btn->m_scaleMultiplier = 1.f;
        btn->setTag(tag);
        return btn;
    }

    void build(bool slide) {
        auto const& all = pages();
        int n = static_cast<int>(all.size());
        s_page = ((s_page % n) + n) % n;
        auto const& page = all[s_page];
        m_accent = accentFor(phud::str("accent"));

        m_buttonMenu->removeAllChildrenWithCleanup(true);
        if (m_chrome) m_chrome->removeFromParentAndCleanup(true);
        if (m_pane) m_pane->removeFromParentAndCleanup(true);

        m_chrome = CCNode::create();
        m_root->addChild(m_chrome, 3);
        m_pane = CCNode::create();
        m_root->addChild(m_pane, 4);
        m_paneMenu = CCMenu::create();
        m_paneMenu->setPosition({0.f, 0.f});
        m_paneMenu->setTouchPriority(-501);
        m_pane->addChild(m_paneMenu, 5);

        m_values.assign(page.rows.size(), nullptr);
        m_sw.assign(page.rows.size(), Sw{});

        this->buildChrome(page, n);
        this->buildRows(page);

        if (slide) {
            m_pane->setPosition({8.f, 0.f});
            m_pane->runAction(CCEaseSineOut::create(CCMoveTo::create(0.14f, CCPoint{0.f, 0.f})));
        }
    }

    void buildChrome(Page const& page, int n) {
        auto const& all = pages();

        // ---- Barra lateral ----
        auto title = text("Practice Suite", m_accent, kSideW - 14.f, 0.8f);
        title->setPosition({kSideCx, 128.f});
        m_chrome->addChild(title);
        auto ver = text(Mod::get()->getVersion().toVString(), kSub, kSideW - 14.f, 0.45f);
        ver->setPosition({kSideCx, 115.f});
        m_chrome->addChild(ver);

        for (int i = 0; i < n; ++i) {
            bool sel = (i == s_page);
            auto btn = this->labelButton(phud::tr(all[i].title), kSideW - 14.f, 20.f,
                sel ? m_accent : kTab, sel ? kDark : kText, menu_selector(PhudGui::onTab));
            btn->m_scaleMultiplier = 1.02f;
            btn->setTag(i);
            btn->setPosition({kSideCx, 94.f - static_cast<float>(i) * 23.f});
            m_buttonMenu->addChild(btn);
        }

        // ---- Cabecera del panel ----
        auto bar = rr(3.f, 14.f, 1.5f, m_accent);
        bar->setPosition({kPanelL + 12.f, 130.f});
        m_chrome->addChild(bar);
        auto pt = text(phud::tr(page.title), kText, 190.f, 0.85f);
        pt->setAnchorPoint({0.f, 0.5f});
        pt->setPosition({kPanelL + 22.f, 130.f});
        m_chrome->addChild(pt);

        auto closeBase = rr(24.f, 24.f, 12.f, kRow);
        auto x = xMark(kText);
        x->setPosition({12.f, 12.f});
        closeBase->addChild(x);
        auto close = CCMenuItemSpriteExtra::create(closeBase, this, menu_selector(PhudGui::onClose));
        close->m_scaleMultiplier = 1.1f;
        close->setPosition({kPanelR - 18.f, 130.f});
        m_buttonMenu->addChild(close);
    }

    void buildRows(Page const& page) {
        float cardW = kPanelW - 16.f;
        float cardL = kPanelCx - cardW / 2.f;
        float cardR = kPanelCx + cardW / 2.f;
        float cursor = 118.f;

        for (size_t i = 0; i < page.rows.size(); ++i) {
            auto const& r = page.rows[i];

            if (r.kind == K::Header) {
                auto t = text(upper(phud::tr(r.label)), m_accent, cardW - 8.f, 0.5f);
                t->setAnchorPoint({0.f, 0.5f});
                t->setPosition({cardL + 4.f, cursor - 11.f});
                m_pane->addChild(t, 1);
                cursor -= 18.f;
                continue;
            }

            float h = r.desc ? 32.f : 22.f;
            float y = cursor - h / 2.f;
            cursor -= h + 2.f;

            auto card = rr(cardW, h, 8.f, kRow);
            card->setPosition({kPanelCx, y});
            m_pane->addChild(card, 0);

            float reserve = r.kind == K::Bool ? 62.f : (r.kind == K::Action ? 92.f : 150.f);
            auto label = text(phud::tr(r.label), kText, cardW - 12.f - reserve, 0.7f);
            label->setAnchorPoint({0.f, 0.5f});
            label->setPosition({cardL + 12.f, r.desc ? y + 7.f : y});
            m_pane->addChild(label, 1);
            if (r.desc) {
                auto d = text(phud::tr(r.desc), kSub, cardW - 12.f - reserve, 0.5f);
                d->setAnchorPoint({0.f, 0.5f});
                d->setPosition({cardL + 12.f, y - 8.f});
                m_pane->addChild(d, 1);
            }

            if (r.kind == K::Bool) {
                CCPoint at{cardR - 12.f - 17.f, y};
                auto offT = rr(34.f, 18.f, 9.f, kOff);
                offT->setPosition(at);
                m_pane->addChild(offT, 1);
                auto onT = rr(34.f, 18.f, 9.f, m_accent);
                onT->setPosition(at);
                m_pane->addChild(onT, 2);
                auto knob = rr(14.f, 14.f, 7.f, kKnob);
                m_pane->addChild(knob, 3);
                m_sw[i] = Sw{onT, knob, at};
                this->applySwitch(i, phud::on(r.key), false);

                auto hit = this->hitArea(cardW, h, static_cast<int>(i), menu_selector(PhudGui::onBool));
                hit->setPosition({kPanelCx, y});
                m_paneMenu->addChild(hit);
            } else if (r.kind == K::Action) {
                auto b = this->labelButton(phud::tr("Reset"), 64.f, 18.f, m_accent, kDark,
                    menu_selector(PhudGui::onResetBall));
                b->setPosition({cardR - 8.f - 32.f, y});
                m_paneMenu->addChild(b);
            } else {
                float fw = 118.f;
                float fx = cardR - 8.f - fw / 2.f;
                auto field = rr(fw, 18.f, 9.f, kField);
                field->setPosition({fx, y});
                m_pane->addChild(field, 1);

                auto la = tri(false, m_accent);
                la->setPosition({fx - fw / 2.f + 13.f, y});
                m_pane->addChild(la, 2);
                auto ra = tri(true, m_accent);
                ra->setPosition({fx + fw / 2.f - 13.f, y});
                m_pane->addChild(ra, 2);

                auto val = text(valueText(r), kText, 74.f, 0.65f);
                val->setPosition({fx, y});
                m_pane->addChild(val, 2);
                m_values[i] = val;

                int base = static_cast<int>(i) * 2;
                auto lb = this->hitArea(30.f, h, base, menu_selector(PhudGui::onStep));
                lb->setPosition({fx - fw / 2.f + 15.f, y});
                m_paneMenu->addChild(lb);
                auto rb = this->hitArea(30.f, h, base + 1, menu_selector(PhudGui::onStep));
                rb->setPosition({fx + fw / 2.f - 15.f, y});
                m_paneMenu->addChild(rb);
                auto mb = this->hitArea(fw - 60.f, h, base + 1, menu_selector(PhudGui::onStep));
                mb->setPosition({fx, y});
                m_paneMenu->addChild(mb);
            }
        }

        // ---- Ayuda en las paginas de imagenes ----
        if (std::string_view(page.title).starts_with("Image")) {
            auto files = phud::listImages();
            auto hint = text(phud::tr(files.empty() ? "No PNGs found. Put PNG files in:" : "Put PNG files in:"),
                kSub, cardW - 20.f, 0.55f);
            hint->setPosition({kPanelCx, cursor - 10.f});
            m_pane->addChild(hint, 1);
            auto path = text(phud::imagesDir(), ccColor3B{110, 116, 146}, cardW - 20.f, 0.45f);
            path->setPosition({kPanelCx, cursor - 22.f});
            m_pane->addChild(path, 1);
        }
    }

    // Mueve la bolita del interruptor y muestra / oculta el fondo de color.
    void applySwitch(size_t i, bool v, bool animate) {
        if (i >= m_sw.size() || !m_sw[i].knob) return;
        auto& s = m_sw[i];
        s.on->setVisible(v);
        CCPoint to{s.at.x + (v ? 8.f : -8.f), s.at.y};
        s.knob->stopAllActions();
        if (animate) s.knob->runAction(CCEaseSineOut::create(CCMoveTo::create(0.12f, to)));
        else s.knob->setPosition(to);
    }

    void onRebuild(float) {
        bool slide = m_slideNext;
        m_slideNext = false;
        this->build(slide);
    }

    // Reconstruir dentro de un callback de boton puede liberar el boton
    // que se esta usando: se hace un instante despues.
    void deferRebuild() {
        this->scheduleOnce(schedule_selector(PhudGui::onRebuild), 0.f);
    }

    void onTab(CCObject* sender) {
        if (m_closing) return;
        int tag = static_cast<CCNode*>(sender)->getTag();
        if (tag == s_page) return;
        s_page = tag;
        m_slideNext = true;
        this->deferRebuild();
    }

    void onBool(CCObject* sender) {
        if (m_closing) return;
        size_t i = static_cast<size_t>(static_cast<CCNode*>(sender)->getTag());
        auto const& r = pages()[s_page].rows[i];
        bool v = !phud::on(r.key);
        (void)Mod::get()->setSettingValue<bool>(r.key, v);
        this->applySwitch(i, v, true);
    }

    void onStep(CCObject* sender) {
        if (m_closing) return;
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
            if (std::strcmp(r.key, "language") == 0 || std::strcmp(r.key, "accent") == 0) {
                this->deferRebuild(); // cambian textos / colores de todo el menu
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
            m_values[i]->limitLabelWidth(74.f, 0.65f, 0.25f);
        }
    }

    void onResetBall(CCObject*) {
        if (m_closing) return;
        phud::resetBall();
    }

    void onClose(CCObject*) {
        if (m_closing) return;
        m_closing = true;
        this->setKeypadEnabled(false);
        m_root->stopAllActions();
        m_root->runAction(CCEaseSineIn::create(CCScaleTo::create(0.12f, 0.8f)));
        this->runAction(CCFadeTo::create(0.12f, 0));
        this->scheduleOnce(schedule_selector(PhudGui::onFinishClose), 0.13f);
    }

    void onFinishClose(float) {
        if (s_open == this) s_open = nullptr;
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

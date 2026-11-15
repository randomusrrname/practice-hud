// Bola flotante arrastrable: toca para abrir el menu de configuracion.

#include "common.hpp"

#include <Geode/ui/OverlayManager.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include <algorithm>
#include <cmath>

using namespace geode::prelude;

namespace phud {
    extern unsigned char const kBallPng[];
    extern unsigned int const kBallPngSize;
}

namespace {
    constexpr float kRadius = 22.f;

    // Crea el sprite del icono desde los bytes incrustados.
    CCSprite* makeIcon() {
        auto img = new CCImage();
        if (!img->initWithImageData(
                const_cast<unsigned char*>(phud::kBallPng),
                static_cast<int>(phud::kBallPngSize), CCImage::kFmtPng)) {
            img->release();
            return nullptr;
        }
        auto tex = CCTextureCache::sharedTextureCache()->addUIImage(img, "phud-ball-icon");
        img->release();
        return tex ? CCSprite::createWithTexture(tex) : nullptr;
    }

    bool pauseOpen() {
        auto scene = CCDirector::get()->getRunningScene();
        if (!scene) return false;
        auto children = scene->getChildren();
        if (!children) return false;
        for (auto child : CCArrayExt<CCNode*>(children)) {
            if (typeinfo_cast<PauseLayer*>(child)) return true;
        }
        return false;
    }
}

class PhudBall : public CCLayer {
protected:
    CCPoint m_touchStart;
    CCPoint m_grab;
    bool m_dragging = false;
    bool m_moved = false;

    bool init() override {
        if (!CCLayer::init()) return false;

        // Icono propio (resources/ball.png), ya sin fondo.
        if (auto icon = makeIcon()) {
            icon->setScale(kRadius * 2.f / std::max(1.f, icon->getContentSize().width));
            this->addChild(icon);
        } else if (auto gear = CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png")) {
            gear->setScale(0.8f);
            this->addChild(gear);
        }

        this->setTouchEnabled(true);
        this->setPosition(this->savedPos());
        this->scheduleUpdate();
        return true;
    }

    static CCPoint defaultPos() {
        auto win = CCDirector::get()->getWinSize();
        return {win.width - 34.f, win.height / 2.f};
    }

    static CCPoint savedPos() {
        auto def = defaultPos();
        auto mod = Mod::get();
        return {
            mod->getSavedValue<float>("ball-x", def.x),
            mod->getSavedValue<float>("ball-y", def.y)
        };
    }

    void savePos() {
        auto p = this->getPosition();
        Mod::get()->setSavedValue<float>("ball-x", p.x);
        Mod::get()->setSavedValue<float>("ball-y", p.y);
    }

    void registerWithTouchDispatcher() override {
        CCDirector::get()->getTouchDispatcher()->addTargetedDelegate(this, -1000, true);
    }

    CCPoint toLocal(CCTouch* touch) {
        auto p = touch->getLocation();
        if (auto parent = this->getParent()) return parent->convertToNodeSpace(p);
        return p;
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent*) override {
        if (!this->isVisible()) return false;
        auto p = this->toLocal(touch);
        auto c = this->getPosition();
        if (std::hypot(p.x - c.x, p.y - c.y) > kRadius + 6.f) return false;
        m_dragging = true;
        m_moved = false;
        m_touchStart = p;
        m_grab = CCPoint{c.x - p.x, c.y - p.y};
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent*) override {
        if (!m_dragging) return;
        auto p = this->toLocal(touch);
        if (std::hypot(p.x - m_touchStart.x, p.y - m_touchStart.y) > 8.f) m_moved = true;
        if (!m_moved) return;
        auto win = CCDirector::get()->getWinSize();
        float x = std::clamp(p.x + m_grab.x, kRadius, win.width - kRadius);
        float y = std::clamp(p.y + m_grab.y, kRadius, win.height - kRadius);
        this->setPosition({x, y});
    }

    void ccTouchEnded(CCTouch*, CCEvent*) override {
        if (!m_dragging) return;
        m_dragging = false;
        if (m_moved) this->savePos();
        else phud::openGui();
    }

    void ccTouchCancelled(CCTouch*, CCEvent*) override {
        if (!m_dragging) return;
        m_dragging = false;
        if (m_moved) this->savePos();
    }

public:
    static PhudBall* create() {
        auto ret = new PhudBall();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void resetPos() {
        auto p = defaultPos();
        this->setPosition(p);
        this->savePos();
    }

    void update(float) override {
        bool show = phud::on("ball-enabled");
        if (show && phud::on("ball-hide-ingame") && PlayLayer::get() && !pauseOpen()) {
            show = false;
        }
        this->setVisible(show);
    }
};

namespace {
    Ref<PhudBall> s_ball;
}

namespace phud {
    // Crea la bola si no existe (o si el juego la limpio al recargar texturas).
    void spawnBall() {
        if (s_ball && s_ball->getParent()) return;
        s_ball = PhudBall::create();
        if (s_ball) OverlayManager::get()->addChild(s_ball);
    }

    void resetBall() {
        if (s_ball) s_ball->resetPos();
    }
}

class $modify(PhudMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        phud::spawnBall();
        return true;
    }
};

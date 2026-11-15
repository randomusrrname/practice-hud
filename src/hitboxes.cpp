// Mostrar hitboxes: dibuja las cajas de colision del jugador y de los objetos.

#include "common.hpp"

#include <Geode/modify/PlayLayer.hpp>

#include <algorithm>
#include <array>

using namespace geode::prelude;

namespace {
    void drawRect(
        CCDrawNode* node, CCRect const& rect,
        ccColor4F const& fill, float border, ccColor4F const& line
    ) {
        std::array<CCPoint, 4> v = {
            CCPoint{rect.getMinX(), rect.getMinY()},
            CCPoint{rect.getMinX(), rect.getMaxY()},
            CCPoint{rect.getMaxX(), rect.getMaxY()},
            CCPoint{rect.getMaxX(), rect.getMinY()}
        };
        node->drawPolygon(v.data(), static_cast<unsigned int>(v.size()), fill, border, line);
    }
}

class $modify(PhudHitboxLayer, PlayLayer) {
    struct Fields {
        CCDrawNode* node = nullptr;
    };

    // Crea (una vez) el nodo donde se dibuja. Va en el mismo padre que el
    // debug draw del juego, asi se mueve con la camara.
    CCDrawNode* phudNode() {
        if (m_fields->node && m_fields->node->getParent()) return m_fields->node;

        CCNode* parent = m_debugDrawNode ? m_debugDrawNode->getParent() : nullptr;
        if (!parent) return nullptr;

        auto node = CCDrawNode::create();
        node->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        node->m_bUseArea = false;
        node->setID("hitboxes"_spr);
        parent->addChild(node, 1402);
        m_fields->node = node;
        return node;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        bool show = phud::on("show-hitboxes");
        if (!show && !m_fields->node) return; // nada que hacer

        auto node = this->phudNode();
        if (!node) return;

        node->clear();
        if (!show) return;

        bool solids = phud::on("hitbox-solids");
        bool hazards = phud::on("hitbox-hazards");
        bool others = phud::on("hitbox-others");
        float fill = static_cast<float>(phud::num("hitbox-fill"));
        constexpr float border = 0.25f;

        auto blue = ccc4f(0.f, 0.25f, 1.f, 1.f);
        auto blueFill = ccc4f(0.f, 0.25f, 1.f, fill);
        auto red = ccc4f(1.f, 0.f, 0.f, 1.f);
        auto redFill = ccc4f(1.f, 0.f, 0.f, fill);
        auto green = ccc4f(0.f, 1.f, 0.f, 1.f);
        auto greenFill = ccc4f(0.f, 1.f, 0.f, fill);
        auto yellow = ccc4f(1.f, 1.f, 0.f, 1.f);
        auto yellowFill = ccc4f(1.f, 1.f, 0.f, fill);
        auto inner = ccc4f(0.f, 1.f, 0.2f, 1.f);
        auto innerFill = ccc4f(0.f, 1.f, 0.2f, fill);

        // Objetos: se recorren solo las secciones que se ven en pantalla.
        int sectionCount = static_cast<int>(m_sections.size());
        int first = std::max(m_leftSectionIndex, 0);
        int firstRow = std::max(m_bottomSectionIndex, 0);

        for (int i = first; i <= m_rightSectionIndex && i < sectionCount; ++i) {
            auto column = m_sections[i];
            auto sizes = i < static_cast<int>(m_sectionSizes.size()) ? m_sectionSizes[i] : nullptr;
            if (!column || !sizes) continue;

            int rows = static_cast<int>(column->size());
            for (int j = firstRow; j <= m_topSectionIndex && j < rows && j < static_cast<int>(sizes->size()); ++j) {
                auto section = column->at(j);
                if (!section) continue;

                int count = std::min(sizes->at(j), static_cast<int>(section->size()));
                for (int k = 0; k < count; ++k) {
                    auto obj = section->at(k);
                    if (!obj || !obj->m_isActivated || obj->m_isGroupDisabled) continue;

                    switch (obj->m_objectType) {
                        case GameObjectType::Decoration:
                            break;

                        case GameObjectType::Solid:
                        case GameObjectType::Slope:
                            if (solids) drawRect(node, obj->getObjectRect(), blueFill, border, blue);
                            break;

                        case GameObjectType::Hazard:
                            if (!hazards || obj == m_anticheatSpike) break;
                            if (obj->m_objectRadius > 0.f) {
                                float radius = std::max(obj->m_scaleX, obj->m_scaleY) * obj->m_objectRadius;
                                node->drawCircle(obj->getPosition(), radius, redFill, border, red, 16);
                            } else {
                                drawRect(node, obj->getObjectRect(), redFill, border, red);
                            }
                            break;

                        default:
                            if (others) drawRect(node, obj->getObjectRect(), greenFill, border, green);
                            break;
                    }
                }
            }
        }

        // Jugadores: caja exterior (amarillo) y caja interior (verde).
        auto drawPlayer = [&](PlayerObject* player) {
            if (!player) return;
            drawRect(node, player->getObjectRect(), yellowFill, border, yellow);
            drawRect(node, player->getObjectRect(0.3f, 0.3f), innerFill, border, inner);
        };
        drawPlayer(m_player1);
        if (m_gameState.m_isDualMode) drawPlayer(m_player2);
    }
};

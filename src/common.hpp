// Cosas compartidas entre todos los archivos del mod.
#pragma once

#include <Geode/Geode.hpp>

#include <array>
#include <string>
#include <vector>

namespace phud {
    inline bool on(char const* key) {
        return geode::Mod::get()->getSettingValue<bool>(key);
    }

    inline double num(char const* key) {
        return geode::Mod::get()->getSettingValue<double>(key);
    }

    inline std::string str(char const* key) {
        return geode::Mod::get()->getSettingValue<std::string>(key);
    }

    // true si en el intento actual se uso un truco que cambia el resultado
    // (noclip que evito una muerte, o speedhack). Se reinicia al reaparecer.
    inline bool g_tripped = false;

    inline bool cheatsOn() {
        return on("noclip") || (on("speedhack-on") && num("speedhack") != 1.0);
    }

    inline bool safeModeActive() {
        return on("safe-mode") && (g_tripped || cheatsOn());
    }

    // Posiciones del HUD / imagenes (el orden coincide con mod.json).
    inline constexpr std::array<char const*, 10> kSlots = {
        "Off", "Top Left", "Top Center", "Top Right",
        "Middle Left", "Center", "Middle Right",
        "Bottom Left", "Bottom Center", "Bottom Right"
    };

    inline int slotIndex(std::string const& s) {
        for (int i = 0; i < static_cast<int>(kSlots.size()); ++i) {
            if (s == kSlots[i]) return i;
        }
        return 0;
    }

    // i18n.cpp: traduce un texto en ingles al idioma elegido.
    std::string tr(std::string const& english);

    // files.cpp: carpeta de imagenes y lista de PNG que hay dentro.
    std::string imagesDir();
    std::vector<std::string> listImages();

    // ball.cpp / gui.cpp
    void spawnBall();
    void resetBall();
    void openGui();
}

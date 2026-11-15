// Cosas compartidas entre todos los archivos del mod.
#pragma once

#include <Geode/Geode.hpp>

namespace phud {
    // Lee un ajuste booleano / numerico del mod.
    inline bool on(char const* key) {
        return geode::Mod::get()->getSettingValue<bool>(key);
    }

    inline double num(char const* key) {
        return geode::Mod::get()->getSettingValue<double>(key);
    }

    // true si en el intento actual se uso un truco que cambia el resultado
    // (noclip que evito una muerte, o speedhack). Se reinicia al reaparecer.
    inline bool g_tripped = false;

    // Hay algun truco activado ahora mismo.
    inline bool cheatsOn() {
        return on("noclip") || (on("speedhack-on") && num("speedhack") != 1.0);
    }

    // El Modo Seguro debe actuar en este momento.
    inline bool safeModeActive() {
        return on("safe-mode") && (g_tripped || cheatsOn());
    }
}

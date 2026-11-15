// Traducciones: la clave es el texto en ingles, el valor es el espanol.
// Sin tildes ni enie en el GUI (la fuente del juego no las trae todas).

#include "common.hpp"

#include <unordered_map>

namespace {
    std::unordered_map<std::string, char const*> const& table() {
        static std::unordered_map<std::string, char const*> t = {
            {"General", "General"}, {"Practice", "Practica"}, {"Cheats", "Trucos"},
            {"Hitboxes", "Hitboxes"}, {"Bypass", "Bypass"}, {"HUD", "HUD"},
            {"HUD Style", "Estilo del HUD"},
            {"Image 1", "Imagen 1"}, {"Image 2", "Imagen 2"}, {"Image 3", "Imagen 3"},
            {"Language", "Idioma"},
            {"Hide ball while playing", "Ocultar bola al jugar"},
            {"Show HUD", "Mostrar HUD"},
            {"Practice music", "Musica en practica"},
            {"Auto practice", "Practica automatica"},
            {"No death effect", "Sin efecto de muerte"},
            {"Safe mode", "Modo seguro"},
            {"Noclip P1", "Noclip J1"}, {"Noclip P2", "Noclip J2"},
            {"Speedhack", "Speedhack"}, {"Speed", "Velocidad"},
            {"Show hitboxes", "Mostrar hitboxes"},
            {"Solids", "Bloques"}, {"Hazards", "Peligros"}, {"Others", "Otros"},
            {"Fill", "Relleno"},
            {"Unlock icons", "Desbloquear iconos"},
            {"Unlock shops", "Desbloquear tiendas"},
            {"Unlock main levels", "Desbloquear niveles"},
            {"No char limit", "Sin limite de texto"},
            {"Max CPS", "CPS maximo"}, {"Attempt", "Intento"}, {"Percent", "Porcentaje"},
            {"Merge CPS + Clicks", "Unir CPS y clicks"},
            {"Text size", "Tamano del texto"}, {"Opacity", "Opacidad"},
            {"Enabled", "Activada"}, {"File", "Archivo"}, {"Position", "Posicion"},
            {"Scale", "Escala"}, {"Offset X", "Desplazar X"}, {"Offset Y", "Desplazar Y"},
            {"Close", "Cerrar"}, {"Reset ball", "Reiniciar bola"},
            {"None", "Ninguno"}, {"Off", "Apagado"},
            {"Top Left", "Arriba izquierda"}, {"Top Center", "Arriba centro"},
            {"Top Right", "Arriba derecha"}, {"Middle Left", "Medio izquierda"},
            {"Center", "Centro"}, {"Middle Right", "Medio derecha"},
            {"Bottom Left", "Abajo izquierda"}, {"Bottom Center", "Abajo centro"},
            {"Bottom Right", "Abajo derecha"},
            {"Put PNG files in:", "Pon tus PNG en:"},
            {"No PNGs found. Put PNG files in:", "No hay PNG. Pon tus PNG en:"},
        };
        return t;
    }
}

namespace phud {
    std::string tr(std::string const& english) {
        if (str("language") != "Espa\xC3\xB1ol") return english;
        auto const& t = table();
        auto it = t.find(english);
        return it == t.end() ? english : std::string(it->second);
    }
}

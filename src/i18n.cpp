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
            {"Text size", "Tamano del texto"}, {"Text opacity", "Opacidad del texto"}, {"Opacity", "Opacidad"},
            {"Enabled", "Activada"}, {"File", "Archivo"}, {"Position", "Posicion"},
            {"Scale", "Escala"}, {"Offset X", "Desplazar X"}, {"Offset Y", "Desplazar Y"},
            {"Close", "Cerrar"}, {"Reset ball", "Reiniciar bola"},
            {"None", "Ninguno"}, {"Off", "Apagado"},
            {"Top Left", "Arriba izquierda"}, {"Top Center", "Arriba centro"},
            {"Top Right", "Arriba derecha"}, {"Middle Left", "Medio izquierda"},
            {"Center", "Centro"}, {"Middle Right", "Medio derecha"},
            {"Bottom Left", "Abajo izquierda"}, {"Bottom Center", "Abajo centro"},
            {"Bottom Right", "Abajo derecha"},
            {"Interface", "Interfaz"}, {"Overlay", "En pantalla"}, {"Practice mode", "Modo practica"},
            {"Safety", "Seguridad"}, {"Gameplay", "Jugabilidad"}, {"Types", "Tipos"},
            {"Local only", "Solo local"}, {"Position", "Posicion"}, {"Style", "Estilo"},
            {"Image", "Imagen"}, {"Reset", "Reiniciar"}, {"Floating ball", "Bola flotante"},
            {"Accent color", "Color de acento"},
            {"Blue", "Azul"}, {"Purple", "Morado"}, {"Pink", "Rosa"}, {"Red", "Rojo"},
            {"Orange", "Naranja"}, {"Green", "Verde"}, {"Cyan", "Cian"},
            {"Menu and HUD language", "Idioma del menu y del HUD"},
            {"Highlight color of this menu", "Color de resaltado del menu"},
            {"Stats on screen while playing", "Estadisticas en pantalla al jugar"},
            {"Reappears when you pause", "Reaparece al pausar"},
            {"Move it back to the default spot", "Vuelve a su lugar original"},
            {"Level music plays in practice mode", "La musica suena en modo practica"},
            {"Enters practice as the level starts", "Entra a practica al empezar el nivel"},
            {"Removes the death explosion", "Quita la explosion al morir"},
            {"Attempts with cheats save no progress", "Intentos con trucos no guardan progreso"},
            {"Pass through obstacles", "Atraviesa obstaculos sin morir"},
            {"Changes the game speed", "Cambia la velocidad del juego"},
            {"Draws the collision boxes", "Dibuja las cajas de colision"},
            {"All icons, colors and trails", "Todos los iconos, colores y trails"},
            {"Opens the secret shops", "Abre las tiendas secretas"},
            {"No coin requirement", "Sin requisito de monedas"},
            {"No limit in text fields", "Sin limite en campos de texto"},
            {"Visual", "Visual"}, {"Effects", "Efectos"}, {"Performance", "Rendimiento"},
            {"Respawn delay", "Retraso al reaparecer"}, {"Delay (seconds)", "Retraso (segundos)"},
            {"Change the time before you respawn", "Cambia el tiempo antes de reaparecer"},
            {"Jump hack", "Jump hack"}, {"Jump in mid-air", "Salta en el aire"},
            {"No mirror portal", "Sin portal espejo"},
            {"Mirror portals do not flip the screen", "Los portales espejo no voltean la pantalla"},
            {"No glow", "Sin brillo"}, {"Removes the glow from objects", "Quita el brillo de los objetos"},
            {"No respawn flash", "Sin flash al reaparecer"},
            {"Removes the flash when you respawn", "Quita el flash al reaparecer"},
            {"No music fade out", "Sin fade de musica"},
            {"Music keeps playing when you die", "La musica sigue al morir"},
            {"FPS bypass", "FPS bypass"}, {"FPS target", "FPS objetivo"},
            {"Raises the frame cap (limited by your screen)", "Sube el limite de FPS (limitado por tu pantalla)"},
            {"Unlock paths", "Desbloquear paths"},
            {"Unlock music customizer", "Desbloquear personalizador de musica"},
            {"No checkpoint limit", "Sin limite de checkpoints"},
            {"Unlimited checkpoints in practice", "Checkpoints ilimitados en practica"},
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

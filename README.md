# Practice Music + HUD (Geode v6 / GD 2.2091)

Mod para Geode con dos funciones:

1. **Musica en modo practica** (activa "Practice Music Sync" del juego).
2. **HUD** con FPS, CPS, CPS maximo, clicks del intento, intento y porcentaje.

## Como compilarlo (GitHub Actions, sin instalar nada)

1. Crea un repositorio nuevo en GitHub y sube **todo el contenido de esta carpeta**
   (incluida la carpeta oculta `.github/workflows/build.yml`).
2. Entra a la pestana **Actions**. El build corre solo con cada push
   (o con **Run workflow**). Tarda unos minutos.
3. Cuando termine en verde, abre la ejecucion y baja el artefacto **Build Output**.
   Adentro esta el archivo `local.practice-hud.geode`.
4. Copia ese `.geode` a la carpeta de mods de Geode y reinicia el juego.

## Si el build falla

- Abre el paso que fallo en Actions y copia el error: con eso se corrige rapido.
- Si el error dice que no encuentra el SDK `v6.0.0-alpha.1`, en
  `.github/workflows/build.yml` cambia `sdk: v6.0.0-alpha.1` por `sdk: nightly`.

## Notas

- La opcion "Musica en modo practica" fuerza *Practice Music Sync* al empezar cada nivel.
  Si la apagas en los ajustes del mod, vuelve a como estaba.
- Los clicks cuentan el boton de salto y se reinician en cada intento.
- No lo subas al indice oficial de Geode: su politica no acepta mods escritos con IA.

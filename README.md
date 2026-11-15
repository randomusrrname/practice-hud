# Practice Suite (Geode v6.0.0-alpha.2 / GD 2.2091)

Mod para Geode con musica en practica, HUD, noclip, speedhack, hitboxes,
desbloqueos y Modo Seguro. No tiene menu propio: todo se activa en los
**ajustes del mod** (Geode > Mods > Practice Suite > ajustes).

## Actualizar el repo desde Termux (si ya tenes ~/practice-hud)

```bash
cd ~/practice-hud
rm -rf src
unzip -o ~/storage/downloads/practice-hud-v2-alpha2.zip -d .
git add -A
git commit -m "v2.0.1: Geode alpha 2"
git push
gh run watch
```

Cuando termine en verde, para bajar el mod:

```bash
ID=$(gh run list -L 1 --json databaseId -q '.[0].databaseId')
gh run download $ID -n "Build Output" -D ~/storage/downloads/practice-hud-build-v2
```

Copia `local.practice-hud.geode` a la carpeta de mods (reemplaza al anterior) y reinicia el juego.

## Si el build falla

```bash
ID=$(gh run list -L 1 --json databaseId -q '.[0].databaseId')
gh run view $ID --log-failed | tail -n 80 > ~/storage/downloads/error.txt
```
y manda `error.txt`.

## Notas

- **Modo Seguro** (activado por defecto): si en un intento usaste noclip (y te salvo de una muerte)
  o speedhack, el juego trata ese intento como prueba: no guarda porcentaje ni completa el nivel.
- **Speedhack**: dejalo apagado al abrir el juego (cambia el tiempo de todo, tambien de los menus).
- **Hitboxes**: "otros objetos" puede bajar los FPS en celulares modestos.
- Los bypass son solo locales: no tocan el servidor ni los rankings.
- No lo subas al indice oficial de Geode: su politica no acepta mods escritos con IA.

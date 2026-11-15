# Practice Suite v3 (Geode v6.0.0-alpha.2 / GD 2.2091)

Bola flotante (arrastrable) que abre el menu del mod. Toca la bola para abrirlo;
las flechas de arriba cambian de pagina. Todo en ingles por defecto; idioma en
General > Language.

## Imagenes en el HUD

1. Abre el menu > pagina "Image 1/2/3": abajo sale la **ruta exacta** de la carpeta `images`.
2. Copia ahi tus PNG (con transparencia).
3. En el menu elige el archivo con las flechas, activa la imagen y elige posicion / escala / opacidad / desplazamiento.

## Actualizar el repo desde Termux

```bash
cd ~/practice-hud
rm -rf src
unzip -o ~/storage/downloads/practice-hud-v3.1.zip -d .
git add -A
git commit -m "v3.0.1: bola flotante, HUD por posiciones, imagenes, idiomas"
git push
gh run watch
```

Cuando termine en verde:

```bash
ID=$(gh run list -L 1 --json databaseId -q '.[0].databaseId')
gh run download $ID -n "Build Output" -D ~/storage/downloads/practice-hud-build-v3
```

Copia `local.practice-hud.geode` a la carpeta de mods (reemplaza al anterior) y reinicia el juego.

## Si el build falla

```bash
ID=$(gh run list -L 1 --json databaseId -q '.[0].databaseId')
gh run view $ID --log-failed | tail -n 80 > ~/storage/downloads/error.txt
```
y manda `error.txt`.

## Notas

- Esta version **no se pudo compilar aqui** (sin SDK): puede haber errores de compilacion pequenos; manda el log y se corrigen.
- Si apagas la bola (`Floating ball` en los ajustes de Geode) solo podras cambiar opciones desde ahi.
- **Modo seguro** (activado por defecto): intentos con noclip/speedhack no guardan progreso.
- Los bypass son solo locales. No lo subas al indice oficial de Geode (no acepta mods hechos con IA).

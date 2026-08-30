# Instalación en SteamOS

SteamOS usa un sistema base inmutable. Para no modificarlo, raylib 5.5 está
instalada como submódulo en `lib/3rd/raylib` y se compila en su propio directorio
`src`. El script de compilación detecta primero esa copia y, si no existe, intenta
usar una instalación accesible mediante `pkg-config`.

Estos pasos se probaron desde el entorno de desarrollo de SteamOS con GCC, Git y
Make disponibles:

```sh
git submodule update --init lib/3rd/raylib
make -C lib/3rd/raylib/src -j2 \
  PLATFORM=PLATFORM_DESKTOP \
  GLFW_LINUX_ENABLE_WAYLAND=TRUE \
  RAYLIB_LIBTYPE=SHARED
```

Luego se compila y ejecuta con los scripts del proyecto:

```sh
sh run/build-linux.sh
sh run/run-linux.sh
```

No es necesario cambiar `LD_LIBRARY_PATH`: el ejecutable guarda la ruta absoluta
de `lib/3rd/raylib/src` mediante RPATH. Los scripts también resuelven la raíz del
repositorio por su propia ubicación, por lo que pueden invocarse desde cualquier
directorio.

Si raylib ya está instalada en otra ruta, se puede evitar la copia local:

```sh
RAYLIB_DIR=/ruta/a/raylib sh run/build-linux.sh
```

Esa ruta debe contener `src/raylib.h` y `src/libraylib.so`.

## Cambios necesarios en este equipo

- Se corrigió el borrado inicial del ejecutable para que una primera compilación
  no terminara silenciosamente por `set -e`.
- Se compiló raylib 5.5 localmente porque el runtime Flatpak no incluía
  `raylib.h` ni `libraylib` y el sistema base de SteamOS no debe modificarse. Se
  habilitó Wayland porque es el servidor gráfico expuesto por este entorno.
- Se eliminaron las rutas fijas `/usr/local/include` y `/usr/local/lib64` de los
  scripts y se añadió detección de la dependencia local o mediante `pkg-config`.

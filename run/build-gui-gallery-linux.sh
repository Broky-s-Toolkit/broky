#!/bin/sh
set -eu

ROOT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cd "$ROOT_DIR"
mkdir -p bin/dbg-linux

RAYLIB_DIR="${RAYLIB_DIR:-${ROOT_DIR}/lib/3rd/raylib}"
if [ -f "${RAYLIB_DIR}/src/raylib.h" ] && [ -f "${RAYLIB_DIR}/src/libraylib.so" ]; then
    RAYLIB_CFLAGS="-I${RAYLIB_DIR}/src"
    RAYLIB_LIBS="-L${RAYLIB_DIR}/src -Wl,-rpath,${RAYLIB_DIR}/src -lraylib"
elif pkg-config --exists raylib 2>/dev/null; then
    RAYLIB_CFLAGS="$(pkg-config --cflags raylib)"
    RAYLIB_LIBDIR="$(pkg-config --variable=libdir raylib)"
    RAYLIB_LIBS="$(pkg-config --libs raylib) -Wl,-rpath,${RAYLIB_LIBDIR}"
else
    echo "raylib no está disponible." >&2
    exit 1
fi

cc -std=c99 -pedantic -Wall -Wextra -Wunused-variable -Wunused-parameter \
    -Wno-unused-value -Wno-unused-function \
    examples/gui-gallery.c -g -ggdb -o bin/dbg-linux/gui-gallery \
    ${RAYLIB_CFLAGS} -Isrc -Ilib ${RAYLIB_LIBS} -lm -pthread

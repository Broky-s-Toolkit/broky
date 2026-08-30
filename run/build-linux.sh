#!/bin/bash

set -e

ROOT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cd "${ROOT_DIR}"

mkdir -p bin/dbg-linux/box2d-obj
rm -f bin/dbg-linux/run

RAYLIB_DIR="${RAYLIB_DIR:-${ROOT_DIR}/lib/3rd/raylib}"
if [ -f "${RAYLIB_DIR}/src/raylib.h" ] && [ -f "${RAYLIB_DIR}/src/libraylib.so" ]; then
    RAYLIB_CFLAGS="-I${RAYLIB_DIR}/src"
    RAYLIB_LIBS="-L${RAYLIB_DIR}/src -Wl,-rpath,${RAYLIB_DIR}/src -lraylib"
elif pkg-config --exists raylib 2>/dev/null; then
    RAYLIB_CFLAGS="$(pkg-config --cflags raylib)"
    RAYLIB_LIBS="$(pkg-config --libs raylib)"
else
    echo "raylib no está disponible." >&2
    echo "Sigue docs/install/steamos.md o define RAYLIB_DIR con la raíz de su código fuente." >&2
    exit 1
fi

BOX2D_LIB="bin/dbg-linux/libbox2d.a"
BOX2D_SRC_DIR="lib/3rd/box2d/src"
BOX2D_INCLUDE_DIR="lib/3rd/box2d/include"
BOX2D_OBJ_DIR="bin/dbg-linux/box2d-obj"
BOX2D_CFLAGS="-g -ggdb -std=c17 -pedantic -Ilib -I${BOX2D_INCLUDE_DIR}"

box2d_rebuild=0
if [ ! -f "${BOX2D_LIB}" ]; then
    box2d_rebuild=1
fi

for src in "${BOX2D_SRC_DIR}"/*.c; do
    obj="${BOX2D_OBJ_DIR}/$(basename "${src}" .c).o"
    if [ ! -f "${obj}" ] || [ "${src}" -nt "${obj}" ]; then
        gcc -Wall -Wextra -Wunused-variable -Wunused-parameter -Wno-unused-value \
            -c "${src}" \
            -o "${obj}" \
            ${BOX2D_CFLAGS}
        box2d_rebuild=1
    fi
done

if [ -f "${BOX2D_LIB}" ] && find "${BOX2D_SRC_DIR}" "${BOX2D_INCLUDE_DIR}" -type f \( -name '*.h' -o -name '*.inl' \) -newer "${BOX2D_LIB}" | grep -q .; then
    for src in "${BOX2D_SRC_DIR}"/*.c; do
        obj="${BOX2D_OBJ_DIR}/$(basename "${src}" .c).o"
        gcc -Wall -Wextra -Wunused-variable -Wunused-parameter -Wno-unused-value \
            -c "${src}" \
            -o "${obj}" \
            ${BOX2D_CFLAGS}
    done
    box2d_rebuild=1
fi

if [ "${box2d_rebuild}" -eq 1 ]; then
    ar rcs "${BOX2D_LIB}" "${BOX2D_OBJ_DIR}"/*.o
fi

gcc -Wall -Wextra -Wunused-variable -Wunused-parameter -Wno-unused-value \
    src/main.c \
    -g -ggdb -o bin/dbg-linux/run -std=c17 -pedantic \
    ${RAYLIB_CFLAGS} -Isrc -Isrc/game -Ilib -Ilib/3rd/box2d/include \
    "${BOX2D_LIB}" ${RAYLIB_LIBS} -lm -pthread

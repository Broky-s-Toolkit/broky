#!/bin/bash

set -e

mkdir -p bin/dbg-linux
mkdir -p bin/dbg-linux/box2d-obj
rm bin/dbg-linux/run > /dev/null 2>&1

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
    -I/usr/local/include -Isrc -Isrc/game -Ilib -Ilib/3rd/box2d/include \
    "${BOX2D_LIB}" -lraylib -lm -pthread

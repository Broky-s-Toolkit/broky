#!/bin/bash
set -e

mkdir -p bin/mac
mkdir -p bin/mac/box2d-obj
cp -r root/. bin/mac/

BOX2D_LIB="bin/mac/libbox2d.a"
BOX2D_SRC_DIR="lib/3rd/box2d/src"
BOX2D_INCLUDE_DIR="lib/3rd/box2d/include"
BOX2D_OBJ_DIR="bin/mac/box2d-obj"
BOX2D_CFLAGS="-g -ggdb -std=c17 -pedantic -Ilib -I${BOX2D_INCLUDE_DIR}"

box2d_rebuild=0
if [ ! -f "${BOX2D_LIB}" ]; then
    box2d_rebuild=1
fi

for src in "${BOX2D_SRC_DIR}"/*.c; do
    obj="${BOX2D_OBJ_DIR}/$(basename "${src}" .c).o"
    if [ ! -f "${obj}" ] || [ "${src}" -nt "${obj}" ]; then
        gcc -w -Wno-unused-value -c "${src}" -o "${obj}" ${BOX2D_CFLAGS}
        box2d_rebuild=1
    fi
done

if [ -f "${BOX2D_LIB}" ] && find "${BOX2D_SRC_DIR}" "${BOX2D_INCLUDE_DIR}" -type f \( -name '*.h' -o -name '*.inl' \) -newer "${BOX2D_LIB}" | grep -q .; then
    for src in "${BOX2D_SRC_DIR}"/*.c; do
        obj="${BOX2D_OBJ_DIR}/$(basename "${src}" .c).o"
        gcc -w -Wno-unused-value -c "${src}" -o "${obj}" ${BOX2D_CFLAGS}
    done
    box2d_rebuild=1
fi

if [ "${box2d_rebuild}" -eq 1 ]; then
    ar rcs "${BOX2D_LIB}" "${BOX2D_OBJ_DIR}"/*.o
fi

gcc src/main.c \
    -Isrc -Isrc/game -Ilib -Ilib/3rd/box2d/include \
    -g -ggdb -w -Wno-unused-value -lraylib -lm -pthread -o bin/mac/run -std=c17 -pedantic \
    "${BOX2D_LIB}"

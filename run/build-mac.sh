#!/bin/bash
mkdir -p bin/mac
cp -r root/. bin/mac/
gcc src/main.c lib/3rd/box2d/src/*.c \
    -Ilib/gui -Ilib/rayext -Ilib/broky -Ilib/3rd/box2d/include \
    -g -ggdb -w -Wno-unused-value -lraylib -lm -pthread -o bin/mac/run -std=c17 -pedantic

#!/bin/bash

mkdir -p bin/dbg-linux
rm bin/dbg-linux/run > /dev/null 2>&1
gcc -Wall -Wextra -Wunused-variable -Wunused-parameter -Wno-unused-value \
    src/main.c \
    lib/3rd/box2d/src/*.c \
    -g -ggdb -o bin/dbg-linux/run -std=c17 -pedantic \
    -I/usr/local/include -Isrc -Isrc/game -Ilib -Ilib/3rd/box2d/include \
    -lraylib -lm -pthread

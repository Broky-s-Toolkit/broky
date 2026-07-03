#!/bin/bash

mkdir -p bin/dbg-linux
rm bin/dbg-linux/run > /dev/null 2>&1
gcc -Wall -Wextra -Wunused-variable -Wunused-parameter \
    src/main.c \
    -g -ggdb -o bin/dbg-linux/run -std=c99 -pedantic \
    -I/usr/local/include -Isrc -Isrc/game -Ilib \
    -lraylib -lm

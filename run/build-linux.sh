#!/bin/bash

mkdir -p bin/dbg-linux
rm bin/dbg-linux/run > /dev/null 2>&1
gcc -Wall -Wextra -Wunused-variable -Wunused-parameter \
    src/main.c \
    -g -ggdb -o bin/dbg-linux/run -std=c99 -pedantic \
    -I/usr/local/include -Ilib/rayext \
    -lraylib -lm \
    $(pkg-config --cflags --libs libavcodec libavformat libavutil libswresample libswscale)

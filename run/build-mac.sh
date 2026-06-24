#!/bin/bash
mkdir -p bin/mac
cp -r root/. bin/mac/
gcc src/main.c -Ilib/gui -Ilib/rayext -Ilib/broky -g -ggdb -w -lraylib -lm -o bin/mac/run -std=c99 -pedantic

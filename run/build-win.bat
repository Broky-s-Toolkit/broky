@echo off
mkdir "bin/dbg-win" 2>NUL
copy "C:\raylib\lib\raylib.dll" "bin/dbg-win" >NUL
gcc src/main.c lib\3rd\box2d\src\*.c -IC:\raylib\include -Isrc -Isrc\game -Ilib -Ilib\3rd\box2d\include -LC:\raylib\lib -g -ggdb -Wno-unused-value -lraylib -lgdi32 -lwinmm -lm -o bin/dbg-win/run.exe -std=c17 -pedantic

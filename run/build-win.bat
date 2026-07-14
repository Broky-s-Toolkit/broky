@echo off
setlocal enabledelayedexpansion

mkdir "bin/dbg-win" 2>NUL
mkdir "bin\dbg-win\box2d-obj" 2>NUL
copy "C:\raylib\lib\raylib.dll" "bin/dbg-win" >NUL

set BOX2D_LIB=bin\dbg-win\libbox2d.a
set BOX2D_SRC_DIR=lib\3rd\box2d\src
set BOX2D_INCLUDE_DIR=lib\3rd\box2d\include
set BOX2D_OBJ_DIR=bin\dbg-win\box2d-obj
set BOX2D_CFLAGS=-g -ggdb -std=c17 -pedantic -Ilib -I%BOX2D_INCLUDE_DIR%
set BOX2D_REBUILD=0

if not exist "%BOX2D_LIB%" set BOX2D_REBUILD=1

for %%F in (%BOX2D_SRC_DIR%\*.c) do (
    set SRC=%%F
    set OBJ=%BOX2D_OBJ_DIR%\%%~nF.o
    if not exist "!OBJ!" (
        gcc -Wno-unused-value -c "!SRC!" -o "!OBJ!" %BOX2D_CFLAGS%
        set BOX2D_REBUILD=1
    ) else (
        for %%T in ("!SRC!") do for %%U in ("!OBJ!") do if %%~tT GTR %%~tU (
            gcc -Wno-unused-value -c "!SRC!" -o "!OBJ!" %BOX2D_CFLAGS%
            set BOX2D_REBUILD=1
        )
    )
)

if exist "%BOX2D_LIB%" (
    for /r "%BOX2D_SRC_DIR%" %%F in (*.h *.inl) do (
        for %%T in ("%%F") do for %%U in ("%BOX2D_LIB%") do if %%~tT GTR %%~tU (
            set BOX2D_REBUILD=1
        )
    )
    for /r "%BOX2D_INCLUDE_DIR%" %%F in (*.h *.inl) do (
        for %%T in ("%%F") do for %%U in ("%BOX2D_LIB%") do if %%~tT GTR %%~tU (
            set BOX2D_REBUILD=1
        )
    )
)

if "%BOX2D_REBUILD%"=="1" (
    for %%F in (%BOX2D_SRC_DIR%\*.c) do (
        set SRC=%%F
        set OBJ=%BOX2D_OBJ_DIR%\%%~nF.o
        if not exist "!OBJ!" gcc -Wno-unused-value -c "!SRC!" -o "!OBJ!" %BOX2D_CFLAGS%
    )
    ar rcs "%BOX2D_LIB%" %BOX2D_OBJ_DIR%\*.o
)

gcc src/main.c -IC:\raylib\include -Isrc -Isrc\game -Ilib -Ilib\3rd\box2d\include -LC:\raylib\lib -g -ggdb -Wno-unused-value -lraylib -lgdi32 -lwinmm -lm -o bin/dbg-win/run.exe -std=c17 -pedantic "%BOX2D_LIB%"

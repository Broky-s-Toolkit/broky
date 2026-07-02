cmake_minimum_required(VERSION 3.16)

project(broky C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(broky
    src/main.c
    src/common.h
    src/env.h
    src/lab.h
    src/lab_wallpaper.h
    src/game/main.h
    src/game/types.h
    src/game/_gui.h
    lib/broky/global.h
    lib/gui/gui.h
    lib/gui/_font_ft.h
    lib/gui/_setup.h
    lib/gui/_grid.h
    lib/gui/_window.h
    lib/gui/_overlay.h
    lib/gui/_controls.h
    lib/rayext/rayext.h
)

target_include_directories(broky PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/broky
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/3rd
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/rayext
    /usr/local/include
)

target_compile_options(broky PRIVATE
    -Wall
    -Wextra
    -Wunused-variable
    -Wunused-parameter
    -pedantic
)

find_library(RAYLIB_LIB raylib PATHS /usr/local/lib)
find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
    pkg_check_modules(FFMPEG QUIET
        libavcodec
        libavformat
        libavutil
        libswresample
        libswscale
    )
endif()

if(RAYLIB_LIB)
    target_link_libraries(broky PRIVATE ${RAYLIB_LIB} m)
else()
    message(WARNING "raylib not found in /usr/local/lib. CLion indexing will work, but builds may fail until raylib is installed.")
endif()

if(FFMPEG_FOUND)
    target_include_directories(broky PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_link_directories(broky PRIVATE ${FFMPEG_LIBRARY_DIRS})
    target_link_libraries(broky PRIVATE ${FFMPEG_LIBRARIES})
    target_compile_options(broky PRIVATE ${FFMPEG_CFLAGS_OTHER})
endif()

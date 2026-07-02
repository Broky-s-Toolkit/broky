cmake_minimum_required(VERSION 3.16)

project(broky C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(broky
    src/main.c
)

target_include_directories(broky PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/3rd
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/rayext
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/broky
    /usr/local/include
)

target_compile_options(broky PRIVATE
    -Wall
    -Wextra
    -Wunused-variable
    -Wunused-parameter
    -pedantic
)

target_compile_options(broky PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Debug>:-ggdb>
)

target_link_libraries(broky PRIVATE
    raylib
    m
)

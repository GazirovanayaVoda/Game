cmake_minimum_required(VERSION 3.22)

project(GameEngine)

set(MAINFILE main.c)

set(VEC libs/vec2.c)
set(COLOR libs/color4f.c)
set(FUNC libs/func.c)
set(COINS libs/coins.c)
set(ENEMY libs/enemy.c)

add_library(vec STATIC ${VEC})
add_library(color STATIC ${COLOR})
add_library(func STATIC ${FUNC})
add_library(coins STATIC ${COINS})
add_library(enemy STATIC ${ENEMY})

add_executable(main  ${MAINFILE})

add_subdirectory(libs/glad/)
target_link_libraries(main PRIVATE glad vec color func coins enemy opengl32 glu32 gdi32)
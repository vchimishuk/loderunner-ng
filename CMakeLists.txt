cmake_minimum_required(VERSION 3.27)
project(LodeRunner)

# add_compile_options(-Wall -Wextra -Wpedantic)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2main)
find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_Image)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})

add_executable(loderunner
                   ai.c
                   animation.c
                   exit.c
                   game.c
                   gold.c
                   guard.c
                   keyhole.c
                   level.c
                   main.c
                   path.c
                   phys.c
                   render.c
                   runner.c
                   texture.c
                   tile.c
                   xmalloc.c)
target_link_libraries(loderunner PRIVATE SDL2::SDL2main)
target_link_libraries(loderunner PRIVATE SDL2_image::SDL2_image)
target_link_libraries(loderunner PRIVATE SDL2::SDL2)

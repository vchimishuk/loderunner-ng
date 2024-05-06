cmake_minimum_required(VERSION 3.27)
project(LodeRunner)

# find_package(SDL REQUIRED)
# if(NOT SDL_FOUND)
# 	message(SEND_ERROR "SDL not found")
# 	return()
# else()
# 	include_directories(${SDL_INCLUDE_DIR})
# endif()

# add_executable(loderunner main.c)
# target_link_libraries(loderunner ${SDL_LIBRARY})



find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2main)

#include_directories(${SDL2_INCLUDE_DIRS})
#find_package(SDL2IMAGE REQUIRED CONFIG REQUIRED COMPONENTS SDL2_Image)
find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_Image)

add_executable(loderunner
                   animation.c
                   exit.c
                   game.c
                   gold.c
                   keyhole.c
                   level.c
                   main.c
                   main.c
                   path.c
                   render.c
                   runner.c
                   texture.c
                   tile.c
                   xmalloc.c)
target_link_libraries(loderunner PRIVATE SDL2::SDL2main)
# target_link_libraries(loderunner PRIVATE SDL2::SDL2_image)
target_link_libraries(loderunner PRIVATE SDL2_image::SDL2_image)
target_link_libraries(loderunner PRIVATE SDL2::SDL2)

# Copyright 2024-2025 Viacheslav Chimishuk <vchimishuk@yandex.ru>
#
# This file is part of loderunner-ng.
#
# loderunner-ng is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# loderunner-ng is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with loderunner-ng. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.27)
project(LodeRunner)

add_compile_options(-std=gnu99 -Wall -Wextra -Wpedantic)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()
if (LINUX)
    # For srandom(), random() & getopt().
    add_compile_definitions(_GNU_SOURCE)
endif()

configure_file(config.h.in config.h)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2main)
find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_Image)
find_package(SDL2_mixer REQUIRED CONFIG REQUIRED COMPONENTS SDL2_mixer)
# 2.8.0 is required for Mix_PauseAudio().
if (SDL2_mixer_VERSION VERSION_LESS "2.8.0")
    message(SEND_ERROR "Minimum 2.8.0 SDL2_mixer version is required but ${SDL2_mixer_VERSION} found")
endif()

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
include_directories(${SDL2_SOUND_INCLUDE_DIR})

add_executable(loderunner-ng
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
                   runner.c
                   sound.c
                   texture.c
                   xmalloc.c)
target_link_libraries(loderunner-ng PRIVATE SDL2::SDL2main)
target_link_libraries(loderunner-ng PRIVATE SDL2_image::SDL2_image)
target_link_libraries(loderunner-ng PRIVATE SDL2_mixer::SDL2_mixer)
target_link_libraries(loderunner-ng PRIVATE SDL2::SDL2)

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

add_compile_options(-std=c99 -Wall -Wextra -Wpedantic -g -fsanitize=address -fno-omit-frame-pointer)
add_link_options(-fsanitize=address)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2main)
find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_Image)
find_package(SDL2_mixer REQUIRED CONFIG REQUIRED COMPONENTS SDL2_mixer)

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
                   phys.c
                   runner.c
                   sound.c
                   texture.c
                   xmalloc.c)
target_link_libraries(loderunner-ng PRIVATE SDL2::SDL2main)
target_link_libraries(loderunner-ng PRIVATE SDL2_image::SDL2_image)
target_link_libraries(loderunner-ng PRIVATE SDL2_mixer::SDL2_mixer)
target_link_libraries(loderunner-ng PRIVATE SDL2::SDL2)

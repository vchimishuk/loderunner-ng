// Copyright 2024-2025 Viacheslav Chimishuk <vchimishuk@yandex.ru>
//
// This file is part of loderunner-ng.
//
// loderunner-ng is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// loderunner-ng is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with loderunner-ng. If not, see <http://www.gnu.org/licenses/>.

#ifndef LEVEL_H_
#define LEVEL_H_

#define NLEVELS 150

#define MAP_WIDTH 28
#define MAP_HEIGHT 16

#define SCORE_DEAD 75
#define SCORE_FINISH_STEP 100
#define SCORE_GOLD 250
#define SCORE_TRAP 75

enum map_tile_t {
    MAP_TILE_BRICK = '#',
    MAP_TILE_EMPTY = ' ',
    MAP_TILE_FALSE = 'X',
    MAP_TILE_GOLD = '$',
    MAP_TILE_GUARD = '0',
    MAP_TILE_HLADDER = 'S',
    MAP_TILE_LADDER = 'H',
    MAP_TILE_ROPE = '-',
    MAP_TILE_RUNNER = '&',
    MAP_TILE_SOLID = '@',
};

struct level {
    int num;
    enum map_tile_t map[MAP_HEIGHT][MAP_WIDTH];
};

struct level *level_init(int n);
void level_destroy(struct level *l);

#endif /* LEVEL_H_ */

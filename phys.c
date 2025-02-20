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

#include "phys.h"

// Check if tile at x:y coordinates has requested type.
bool is_tile(struct game *game, int x, int y, enum map_tile_t t)
{
    return game->map[y][x]->curt == t;
}

// Returns true if runner can move to tile with x:y coordinates.
bool can_move(struct game *game, int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }

    return is_tile(game, x, y, MAP_TILE_EMPTY)
        || is_tile(game, x, y, MAP_TILE_LADDER)
        || is_tile(game, x, y, MAP_TILE_ROPE);
}

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

#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "guard.h"
#include "level.h"
#include "runner.h"

#define MAX_GOLD 32
#define MAX_GUARDS 12

/*
 * Game map single tile representation.
 */
struct map_tile {
    // Base animation for this tile, animation displayed by default. E.g. brick.
    struct animation *basea;
    // Currently displayed animation on the current map tile. Usually same as
    // base animation but can be different in some periods of the game. For
    // example, when runner digs a hole current animatio is set to hole animation
    // instead of brick animation.
    struct animation *cura;
    // Tile's base type.
    enum map_tile_t baset;
    // Tile's current type. Can be different from base type at some periods
    // during game. For example, when hole is dug tile's type is changed from
    // brick to empty for period of time when hole is active. Returned back to
    // base_t after that.
    enum map_tile_t curt;
    // X screen coordinate to draw current animation at. It is not always the
    // same as actual map tile coordinate as some animations (like hole
    // animation) takes space of two map tiles.
    int x;
    // Y screen coordinate to draw current animation at.
    int y;
};

struct ground_tile {
    struct animation *a;
    int x;
    int y;
};

enum game_state {
    // Runner is dead or reached end of the current leve. Keyhole animation
    // is shown.
    GSTATE_END,
    // Game is running, user can control runner.
    GSTATE_RUN,
    // Game round is going to start, keyhole is shown or waiting for initial
    // user's command.
    GSTATE_START,
};

/*
 * Game represents single level playing process. It stores gaming state of the
 * current level, like active textures, their location, location of runner,
 * guards, gold and so on.
 */
struct game {
    enum game_state state;
    float keyhole;
    struct level *level;
    int lives;
    struct map_tile *map[MAP_HEIGHT][MAP_WIDTH];
    struct ground_tile *ground[MAP_WIDTH];
    struct sprite **info_score;
    struct sprite **info_lives;
    struct sprite **info_level;
    struct runner *runner;
    struct guard **guards;
    int nguards;
    struct gold *gold[MAX_GOLD];
    int ngold;
    bool won;
    // Current user score.
    int score;
    // Storage for iteration counter when accruing level bonus scores.
    int level_score_iter;
};

struct game *game_init(struct level *lvl);
bool game_tick(struct game *game, int key);
void game_render(struct game *game, SDL_Renderer *renderer);
void game_destroy(struct game *game);
void game_score(struct game *game, int score);
struct guard *game_guard_get(struct game *g, int x, int y);

#endif /* GAME_H_ */

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

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "texture.h"

#define TILE_GROUND_HEIGHT 20
#define TILE_GROUND_WIDTH 40
#define TILE_MAP_HEIGHT 44
#define TILE_MAP_WIDTH 40
#define TILE_TEXT_HEIGHT 44
#define TILE_TEXT_WIDTH 40

enum animation_t {
    ANIMATION_BRICK,
    ANIMATION_GOLD,
    ANIMATION_GROUND,
    ANIMATION_GUARD_CLIMB_LEFT,
    ANIMATION_GUARD_CLIMB_RIGHT,
    ANIMATION_GUARD_FALL_LEFT,
    ANIMATION_GUARD_FALL_RIGHT,
    ANIMATION_GUARD_LEFT,
    ANIMATION_GUARD_REBORN,
    ANIMATION_GUARD_RIGHT,
    ANIMATION_GUARD_TRAP_LEFT,
    ANIMATION_GUARD_TRAP_RIGHT,
    ANIMATION_GUARD_UPDOWN,
    ANIMATION_HOLE_FILL,
    ANIMATION_LADDER,
    ANIMATION_NONE,
    ANIMATION_ROPE,
    ANIMATION_RUNNER_CLIMB_LEFT,
    ANIMATION_RUNNER_CLIMB_RIGHT,
    ANIMATION_RUNNER_DIG_LEFT,
    ANIMATION_RUNNER_DIG_RIGHT,
    ANIMATION_RUNNER_FALL_LEFT,
    ANIMATION_RUNNER_FALL_RIGHT,
    ANIMATION_RUNNER_HOLE_LEFT,
    ANIMATION_RUNNER_HOLE_RIGHT,
    ANIMATION_RUNNER_LEFT,
    ANIMATION_RUNNER_RIGHT,
    ANIMATION_RUNNER_UPDOWN,
    ANIMATION_SOLID,
};

struct sprite {
    SDL_Texture *texture;
    int x;
    int y;
    int w;
    int h;
    // Number of frames to display current sprite for before moving
    // to the next srpite.
    int frames;
};

struct animation {
    struct sprite **sprites;
    struct sprite **cur;
    int frame;
};

struct animation *animation_init(enum animation_t t);
void animation_destroy(struct animation *a);
bool animation_tick(struct animation *a);
void animation_reset(struct animation *a);
void animation_render(SDL_Renderer *renderer, struct animation *a, int x, int y);
void animation_render_sprite(SDL_Renderer *renderer, struct sprite *s, int x, int y);

#endif /* ANIMATION_H_ */

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

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "texture.h"
#include "animation.h"
#include "exit.h"
#include "xmalloc.h"

static struct sprite *animation_sprite_init(enum texture tx, int x, int y,
    int w, int h, int frames)
{
    struct sprite *s = xmalloc(sizeof(struct sprite));
    s->texture = texture_get(tx);
    s->x = x;
    s->y = y;
    s->w = w;
    s->h = h;
    s->frames = frames;

    return s;
}

static struct sprite **sprites_init(int size)
{
    return xmalloc(sizeof(struct sprite) * size);
}

static struct sprite *runner_sprite_init(int n, int frames)
{
    int r = n / 9;
    int c = n % 9;

    return animation_sprite_init(TEXTURE_RUNNER,
        c * TILE_MAP_WIDTH, r * TILE_MAP_HEIGHT,
        TILE_MAP_WIDTH, TILE_MAP_HEIGHT,
        frames);
}

static struct sprite *guard_sprite_init(int n, int frames)
{
    int r = n / 11;
    int c = n % 11;

    return animation_sprite_init(TEXTURE_GUARD,
        c * TILE_MAP_WIDTH, r * TILE_MAP_HEIGHT,
        TILE_MAP_WIDTH, TILE_MAP_HEIGHT,
        frames);
}

static struct sprite *hole_sprite_init(int n, bool tall, int frames)
{
    int r = n / 9;
    int c = n % 9;
    int t = tall ? 2 : 1;

    return animation_sprite_init(TEXTURE_HOLE,
        c * TILE_MAP_WIDTH, r * TILE_MAP_HEIGHT * t,
        TILE_MAP_WIDTH, TILE_MAP_HEIGHT * t,
        frames);
}

struct animation *animation_init(enum animation_t t)
{
    struct animation *a = xmalloc(sizeof(struct animation));

    switch (t) {
    case ANIMATION_BRICK:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_BRICK, 0, 0,
            TILE_MAP_WIDTH, TILE_MAP_HEIGHT, 1);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_GOLD:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_GOLD, 0, 0,
            TILE_MAP_WIDTH, TILE_MAP_HEIGHT, 1);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_GROUND:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_GROUND, 0, 0,
            TILE_GROUND_WIDTH, TILE_GROUND_HEIGHT, 1);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_GUARD_CLIMB_LEFT:
        a->sprites = sprites_init(4);
        a->sprites[0] = guard_sprite_init(25, 1);
        a->sprites[1] = guard_sprite_init(26, 2);
        a->sprites[2] = guard_sprite_init(27, 2);
        a->sprites[3] = NULL;
        break;
    case ANIMATION_GUARD_CLIMB_RIGHT:
        a->sprites = sprites_init(4);
        a->sprites[0] = guard_sprite_init(22, 1);
        a->sprites[1] = guard_sprite_init(23, 2);
        a->sprites[2] = guard_sprite_init(24, 2);
        a->sprites[3] = NULL;
        break;
    case ANIMATION_GUARD_FALL_LEFT:
        a->sprites = sprites_init(2);
        a->sprites[0] = guard_sprite_init(30, 1);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_GUARD_FALL_RIGHT:
        a->sprites = sprites_init(2);
        a->sprites[0] = guard_sprite_init(8, 1);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_GUARD_LEFT:
        a->sprites = sprites_init(4);
        a->sprites[0] = guard_sprite_init(3, 2);
        a->sprites[1] = guard_sprite_init(4, 2);
        a->sprites[2] = guard_sprite_init(5, 2);
        a->sprites[3] = NULL;
        break;
    case ANIMATION_GUARD_REBORN:
        a->sprites = sprites_init(3);
        a->sprites[0] = guard_sprite_init(28, 6);
        a->sprites[1] = guard_sprite_init(29, 2);
        a->sprites[2] = NULL;
        break;
    case ANIMATION_GUARD_RIGHT:
        a->sprites = sprites_init(4);
        a->sprites[0] = guard_sprite_init(0, 2);
        a->sprites[1] = guard_sprite_init(1, 2);
        a->sprites[2] = guard_sprite_init(2, 2);
        a->sprites[3] = NULL;
        break;
    case ANIMATION_GUARD_TRAP_LEFT:
        a->sprites = sprites_init(7);
        // LodeRunner TotalRecall uses totally 18 frames with 0.3 speed.
        // In this case we should have 60 frames in total instead of original
        // 18 (18 * 3.33333) but it looks too long comparing to TotalRecal.
        // So, I simply tune number of frames here to make it have the same
        // speed.
        a->sprites[0] = guard_sprite_init(30, 37);
        a->sprites[1] = guard_sprite_init(31, 3);
        a->sprites[2] = guard_sprite_init(32, 3);
        a->sprites[3] = guard_sprite_init(31, 3);
        a->sprites[4] = guard_sprite_init(32, 3);
        a->sprites[5] = guard_sprite_init(30, 3);
        a->sprites[6] = NULL;
        break;
    case ANIMATION_GUARD_TRAP_RIGHT:
        a->sprites = sprites_init(7);
        a->sprites[0] = guard_sprite_init(8, 37);
        a->sprites[1] = guard_sprite_init(9, 3);
        a->sprites[2] = guard_sprite_init(10, 3);
        a->sprites[3] = guard_sprite_init(9, 3);
        a->sprites[4] = guard_sprite_init(10, 3);
        a->sprites[5] = guard_sprite_init(8, 3);
        a->sprites[6] = NULL;
        break;
    case ANIMATION_GUARD_UPDOWN:
        a->sprites = sprites_init(3);
        a->sprites[0] = guard_sprite_init(6, 1);
        a->sprites[1] = guard_sprite_init(7, 1);
        a->sprites[2] = NULL;
        break;
    case ANIMATION_HOLE_FILL:
        a->sprites = sprites_init(5);
        a->sprites[0] = hole_sprite_init(26, false, 166);
        a->sprites[1] = hole_sprite_init(17, false, 8);
        a->sprites[2] = hole_sprite_init(8, false, 8);
        a->sprites[3] = hole_sprite_init(35, false, 4);
        a->sprites[4] = NULL;
        break;
    case ANIMATION_LADDER:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_LADDER, 0, 0,
            TILE_MAP_WIDTH, TILE_MAP_HEIGHT, 1);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_ROPE:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_ROPE, 0, 0,
            TILE_MAP_WIDTH, TILE_MAP_HEIGHT, 1);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_RUNNER_CLIMB_LEFT:
        a->sprites = sprites_init(4);
        a->sprites[0] = runner_sprite_init(21, 1);
        a->sprites[1] = runner_sprite_init(22, 2);
        a->sprites[2] = runner_sprite_init(23, 2);
        a->sprites[3] = NULL;
        break;
    case ANIMATION_RUNNER_CLIMB_RIGHT:
        a->sprites = sprites_init(4);
        a->sprites[0] = runner_sprite_init(18, 1);
        a->sprites[1] = runner_sprite_init(19, 2);
        a->sprites[2] = runner_sprite_init(20, 2);
        a->sprites[3] = NULL;
        break;
    case ANIMATION_RUNNER_DIG_LEFT:
        // Repeat dig animation as many sprites as hole digging animation
        // lasts so they have the same duration.
        a->sprites = sprites_init(2);
        a->sprites[0] = runner_sprite_init(25, 11);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_RUNNER_DIG_RIGHT:
        // Repeat dig animation as many sprites as hole digging animation
        // lasts so they have the same duration.
        a->sprites = sprites_init(2);
        a->sprites[0] = runner_sprite_init(24, 11);
        a->sprites[2] = NULL;
        break;
    case ANIMATION_RUNNER_FALL_LEFT:
        a->sprites = sprites_init(2);
        a->sprites[0] = runner_sprite_init(26, 1);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_RUNNER_FALL_RIGHT:
        a->sprites = sprites_init(2);
        a->sprites[0] = runner_sprite_init(8, 1);
        a->sprites[1] = NULL;
        break;
    case ANIMATION_RUNNER_HOLE_LEFT:
        a->sprites = sprites_init(9);
        a->sprites[0] = hole_sprite_init(0, true, 1);
        a->sprites[1] = hole_sprite_init(1, true, 1);
        a->sprites[2] = hole_sprite_init(2, true, 2);
        a->sprites[3] = hole_sprite_init(3, true, 1);
        a->sprites[4] = hole_sprite_init(4, true, 2);
        a->sprites[5] = hole_sprite_init(5, true, 1);
        a->sprites[6] = hole_sprite_init(6, true, 2);
        a->sprites[7] = hole_sprite_init(7, true, 1);
        a->sprites[8] = NULL;
        break;
    case ANIMATION_RUNNER_HOLE_RIGHT:
        a->sprites = sprites_init(9);
        a->sprites[0] = hole_sprite_init(9, true, 1);
        a->sprites[1] = hole_sprite_init(10, true, 1);
        a->sprites[2] = hole_sprite_init(11, true, 2);
        a->sprites[3] = hole_sprite_init(12, true, 1);
        a->sprites[4] = hole_sprite_init(13, true, 2);
        a->sprites[5] = hole_sprite_init(14, true, 1);
        a->sprites[6] = hole_sprite_init(15, true, 2);
        a->sprites[7] = hole_sprite_init(16, true, 1);
        a->sprites[8] = NULL;
        break;
    case ANIMATION_RUNNER_LEFT:
        a->sprites = sprites_init(4);
        a->sprites[0] = runner_sprite_init(3, 1);
        a->sprites[1] = runner_sprite_init(4, 1);
        a->sprites[2] = runner_sprite_init(5, 1);
        a->sprites[3] = NULL;
        break;
    case ANIMATION_RUNNER_RIGHT:
        a->sprites = sprites_init(4);
        a->sprites[0] = runner_sprite_init(0, 1);
        a->sprites[1] = runner_sprite_init(1, 1);
        a->sprites[2] = runner_sprite_init(2, 1);
        a->sprites[3] = NULL;
        break;
    case ANIMATION_RUNNER_UPDOWN:
        a->sprites = sprites_init(3);
        a->sprites[0] = runner_sprite_init(6, 1);
        a->sprites[1] = runner_sprite_init(7, 1);
        a->sprites[2] = NULL;
        break;
    case ANIMATION_SOLID:
        a->sprites = sprites_init(2);
        a->sprites[0] = animation_sprite_init(TEXTURE_SOLID, 0, 0,
            TILE_MAP_WIDTH, TILE_MAP_HEIGHT, 1);
        a->sprites[1] = NULL;
        break;
    default:
        die("illegal state");
    }

    animation_reset(a);

    return a;
}

void animation_destroy(struct animation *a)
{
    for (struct sprite **s = a->sprites; *s != NULL; s++) {
        free(*s);
    }
    free(a->sprites);
    free(a);
}

/*
 * Switch to the next animation sprite.
 * Returns true it end of the animation has been reached and moved back
 * to the first sprite.
 */
bool animation_tick(struct animation *a)
{
    bool replay = false;

    if (a->frame) {
        // Keep showing the same sprite for the number of frames defined by
        // sprite's `frames` field.
        a->frame--;
    } else {
        // Move to the next sprite.
        a->cur++;
        if (*(a->cur) == NULL) {
            replay = true;
            animation_reset(a);
        } else {
            a->frame = (*(a->cur))->frames;
        }
    }

    return replay;
}

/*
 * Reset animation to the first sprite.
 */
void animation_reset(struct animation *a)
{
    a->cur = a->sprites;
    a->frame = (*(a->cur))->frames;
}

/*
 * Render current sprite of the animation.
 */
void animation_render(SDL_Renderer *renderer, struct animation *a, int x, int y)
{
    animation_render_sprite(renderer, *(a->cur), x, y);
}

void animation_render_sprite(SDL_Renderer *renderer, struct sprite *s, int x, int y)
{
    SDL_Rect src;
    src.x = s->x;
    src.y = s->y;
    src.w = s->w;
    src.h = s->h;
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = s->w;
    dst.h = s->h;
    if (SDL_RenderCopy(renderer, s->texture, &src, &dst) < 0) {
        die_sdl("SDL_RenderCopy");
    }
}

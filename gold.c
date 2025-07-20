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

#include "game.h"
#include "gold.h"
#include "phys.h"
#include "xmalloc.h"

struct gold *gold_init(int x, int y)
{
    struct gold *g = xmalloc(sizeof(struct gold));
    g->sx = x;
    g->sy = y;
    g->animation = animation_init(ANIMATION_GOLD);
    gold_reset(g);

    return g;
}

void gold_destroy(struct gold *gold)
{
    animation_destroy(gold->animation);
    free(gold);
}

void gold_reset(struct gold *g)
{
    g->x = g->sx;
    g->y = g->sy;
    g->visible = true;
    g->lost = false;
}

struct gold *gold_get(struct game *g, int x, int y)
{
    for (int i = 0; i < g->ngold; i++) {
        struct gold *gl = g->gold[i];

        if (!gl->lost && gl->visible && gl->x == x && gl->y == y) {
            return gl;
        }
    }

    return NULL;
}

/*
 * Returns gold at runner or guard position if can.
 */
struct gold *gold_pickup(struct game *game, int x, int y, int tx, int ty)
{
    struct gold *g = gold_get(game, x, y);
    if (g != NULL) {
        struct guard *gd = game_guard_get(game, x, y + 1);
        // When gold hangs in air then pick it up only when falling on it
        // from above.
        if (!is_tile(game, x, y + 1, MAP_TILE_EMPTY)
            || (ty < 0 && ty >= -MOVE_DY)
            || gd != NULL) {
            g->visible = false;
            return g;
        }
    }

    return NULL;
}

void gold_drop(struct gold *g, int x, int y)
{
    g->x = x;
    g->y = y;
    g->visible = true;
}

void gold_lose(struct game *gm, struct gold *gld)
{
    gm->lost_gold += 1;
    gld->lost = true;
    gld->visible = false;
}

#include "game.h"
#include "gold.h"
#include "tile.h"
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

void gold_reset(struct gold *g)
{
    g->x = g->sx;
    g->y = g->sy;
    g->visible = true;
}

void gold_destroy(struct gold *gold)
{
    // TODO: animation_destroy(gold->animation);
    free(gold);
}

struct gold *gold_get(struct game *g, int x, int y)
{
    for (int i = 0; i < g->ngold; i++) {
        struct gold *gl = g->gold[i];

        if (gl->visible && gl->x == x && gl->y == y) {
            return gl;
        }
    }

    return NULL;
}

struct gold *gold_pickup(struct game *game, int x, int y, int tx, int ty)
{
    struct gold *g = gold_get(game, x, y);
    if (g != NULL
        && abs(0 - tx) <= TILE_MAP_WIDTH / 4
        && abs(0 - ty) <= TILE_MAP_HEIGHT / 4) {
        g->visible = false;
        return g;
    }

    return NULL;
}

void gold_drop(struct gold *g, int x, int y)
{
    g->x = x;
    g->y = y;
    g->visible = true;
}

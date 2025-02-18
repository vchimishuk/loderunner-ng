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
    if (g != NULL
        && ((tx == 0 && ty >= 0 && ty < MOVE_DY)
            || (ty == 0 && tx >= 0 && tx < MOVE_DX))) {
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

void gold_lose(struct gold *g)
{
    g->lost = true;
}

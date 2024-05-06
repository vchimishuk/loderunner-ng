#include "gold.h"
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

#include "runner.h"
#include "texture.h"
#include "tile.h"
#include "xmalloc.h"

/* #define TILE_MAP_WIDTH 40 */
/* #define TILE_MAP_HEIGHT 44 */

static struct tile *tile_init(int n)
{
    int r = n / 9;
    int c = n % 9;

    struct tile *t = xmalloc(sizeof(struct tile));
    t->texture = texture_get(TEXTURE_RUNNER);
    t->x = 0;
    t->y = 0;
    t->tx = c * TILE_MAP_WIDTH;
    t->ty = r * TILE_MAP_HEIGHT;
    t->tw = TILE_MAP_WIDTH;
    t->th = TILE_MAP_HEIGHT;

    return t;
}

struct runner *runner_init()
{
    struct runner *r = xmalloc(sizeof(struct runner));
    r->x = 0;
    r->y = 0;
    r->tx = 0;
    r->ty = 0;
    r->ltiles = xmalloc(sizeof(struct tile *) * 4);
    r->ltiles[0] = tile_init(3);
    r->ltiles[1] = tile_init(4);
    r->ltiles[2] = tile_init(5);
    r->ltiles[3] = NULL;
    r->rtiles = xmalloc(sizeof(struct tile *) * 4);
    r->rtiles[0] = tile_init(0);
    r->rtiles[1] = tile_init(1);
    r->rtiles[2] = tile_init(2);
    r->rtiles[3] = NULL;
    r->tile = r->rtiles[0];

    return r;
}

void runner_destroy(struct runner *runner)
{
    // TODO:
}

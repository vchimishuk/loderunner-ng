#include "tile.h"

struct tile *tile_init(enum texture t, int row, int col)
{
    struct tile *tl = xmalloc(sizeof(struct tile));
    tl->texture = texture_get(t);
    tl->x = col * TILE_MAP_WIDTH;
    tl->y = row * TILE_MAP_HEIGHT;
    tl->tx = 0;
    tl->ty = 0;
    tl->tw = TILE_MAP_WIDTH;
    tl->th = TILE_MAP_HEIGHT;

    return tl;
}

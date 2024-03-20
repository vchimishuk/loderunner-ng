#ifndef TILE_H_
#define TILE_H_

// TODO: Remove tile.h & tile.c.
#include <SDL2/SDL.h>
#include "texture.h"
#include "xmalloc.h"

/* #define TILE_DIG_HEIGHT 88 */
/* #define TILE_DIG_WIDTH 40 */
#define TILE_GROUND_HEIGHT 20
#define TILE_GROUND_WIDTH 40
// TODO: Use single tile constants for map and text?
#define TILE_MAP_HEIGHT 44
#define TILE_MAP_WIDTH 40
#define TILE_TEXT_HEIGHT 44
#define TILE_TEXT_WIDTH 40

// TODO: Deprecated.
//       Looks like we need to remove tile.h and tile.c
struct tile {
    SDL_Texture *texture;
    int x;
    int y;
    int tx;
    int ty;
    int tw;
    int th;
};

struct tile *tile_init(enum texture t, int row, int col);

#endif /* TILE_H_ */

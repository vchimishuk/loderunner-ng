#ifndef TILE_H_
#define TILE_H_

#include <SDL2/SDL.h>

#define TILE_MAP_WIDTH 40
#define TILE_MAP_HEIGHT 44
#define TILE_GROUND_WIDTH 40
#define TILE_GROUND_HEIGHT 20
#define TILE_TEXT_WIDTH 40
#define TILE_TEXT_HEIGHT 44

struct tile {
    SDL_Texture *texture;
    int x;
    int y;
    int tx;
    int ty;
    int tw;
    int th;
};

/* struct tile_ground { */
/*     SDL_Texture *texture; */
/*     int x; */
/*     int y; */
/*     int w; */
/*     int h; */
/* }; */

/* /\* */
/*  * Representation of level map tiles like bricks, blocks, ladders, etc. */
/*  *\/ */
/* struct tile_map { */
/*     SDL_Texture *texture; */
/*     int x; */
/*     int y; */
/*     int w; */
/*     int h; */
/*     // visible, etc */
/* }; */

/* /\* */
/*  * Representation of text character (letter or number). */
/*  *\/ */
/* struct tile_text { */
/*     SDL_Texture *texture; */
/*     char ch; */
/*     int x; */
/*     int y; */
/*     int w; */
/*     int h; */
/* }; */

#endif /* TILE_H_ */

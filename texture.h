#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL.h>

enum texture {
    TEXTURE_BRICK,
    TEXTURE_EMPTY,
    TEXTURE_GOLD,
    TEXTURE_GROUND,
    TEXTURE_GUARD,
    TEXTURE_HLADDER,
    TEXTURE_HOLE,
    TEXTURE_LADDER,
    TEXTURE_ROPE,
    TEXTURE_RUNNER,
    TEXTURE_SOLID,
    TEXTURE_TEXT,
    TEXTURE_TRAP,
    // Keep it last.
    TEXTURE_SIZE,
};

SDL_Texture *texture_load(SDL_Renderer *renderer, char *file);
void texture_init(SDL_Renderer *renderer);
void texture_destroy();
SDL_Texture *texture_get(enum texture t);

#endif /* TEXTURE_H_ */

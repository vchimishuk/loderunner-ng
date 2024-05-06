#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "exit.h"
#include "level.h"
#include "path.h"
#include "texture.h"

#define TEXTURES_DIR "./textures"

static SDL_Texture *textures[TEXTURE_SIZE] = {NULL};

/*
 * Load texture image from file.
 * Calls die() on error.
 *
 * TODO: Make it private?
 */
/* static */ SDL_Texture *texture_load(SDL_Renderer *renderer, char *file)
{
    char *path = path_join(TEXTURES_DIR, file);
    SDL_Texture *texture = IMG_LoadTexture(renderer, path);
    if (texture == NULL) {
        die("failed to load texture: %s", IMG_GetError());
    }

    return texture;
}

void texture_init(SDL_Renderer *renderer)
{
    textures[TEXTURE_BRICK] = texture_load(renderer, "brick.png");
    textures[TEXTURE_EMPTY] = NULL;
    textures[TEXTURE_GOLD] = texture_load(renderer, "gold.png");
    // texture_map[TILE_GUARD] = texture_load(renderer, ".png");
    // texture_map[TILE_HLADDER] = texture_load(renderer, ".png");
    textures[TEXTURE_HOLE] = texture_load(renderer, "hole.png");
    textures[TEXTURE_LADDER] = texture_load(renderer, "ladder.png");
    textures[TEXTURE_ROPE] = texture_load(renderer, "rope.png");
    textures[TEXTURE_RUNNER] = texture_load(renderer, "runner.png");
    textures[TEXTURE_SOLID] = texture_load(renderer, "solid.png");
    // texture_map[TILE_TRAP] = texture_load(renderer, ".png");
    textures[TEXTURE_GROUND] = texture_load(renderer, "ground.png");
    textures[TEXTURE_TEXT] = texture_load(renderer, "text.png");
}

SDL_Texture *texture_get(enum texture t)
{
    return textures[t];
}

void texture_destroy()
{
    for (int i = 0; i < TEXTURE_SIZE; i++) {
        SDL_DestroyTexture(textures[i]);
        textures[i] = NULL;
    }
}

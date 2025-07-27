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

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "config.h"
#include "exit.h"
#include "level.h"
#include "path.h"
#include "texture.h"

static SDL_Texture *textures[TEXTURE_SIZE] = {NULL};

/*
 * Load texture image from file.
 * Calls die() on error.
 */
SDL_Texture *texture_load(SDL_Renderer *renderer, char *file)
{
    char *path = path_join(TEXTURES_DIR, file);
    SDL_Texture *texture = IMG_LoadTexture(renderer, path);
    free(path);

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
    textures[TEXTURE_GUARD] = texture_load(renderer, "guard.png");
    // texture_map[TILE_HLADDER] = texture_load(renderer, ".png");
    textures[TEXTURE_HOLE] = texture_load(renderer, "hole.png");
    textures[TEXTURE_LADDER] = texture_load(renderer, "ladder.png");
    textures[TEXTURE_PAUSED] = texture_load(renderer, "paused.png");
    textures[TEXTURE_ROPE] = texture_load(renderer, "rope.png");
    textures[TEXTURE_RUNNER] = texture_load(renderer, "runner.png");
    textures[TEXTURE_SOLID] = texture_load(renderer, "solid.png");
    // texture_map[TILE_TRAP] = texture_load(renderer, ".png");
    textures[TEXTURE_GROUND] = texture_load(renderer, "ground.png");
    textures[TEXTURE_TEXT] = texture_load(renderer, "text.png");
}

void texture_destroy(void)
{
    for (int i = 0; i < TEXTURE_SIZE; i++) {
        SDL_DestroyTexture(textures[i]);
        textures[i] = NULL;
    }
}

SDL_Texture *texture_get(enum texture t)
{
    return textures[t];
}

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
// Original, untinted RGBA pixels for each persistent texture. Kept so the
// color mode can be re-applied at runtime without reloading files.
static SDL_Surface *sources[TEXTURE_SIZE] = {NULL};
static enum color_mode current_mode = COLOR_ORIGINAL;

/*
 * Rewrite dst from src, applying the given color mode. Both surfaces must be
 * RGBA32 and have identical dimensions. dst and src may alias.
 *
 * Monochrome modes collapse each pixel to its luminance, then paint that
 * brightness in the phosphor's color -- matching how a monochrome CRT renders
 * a signal regardless of the source's original hue.
 */
static void apply_color_mode(SDL_Surface *dst, SDL_Surface *src,
    enum color_mode mode)
{
    // Phosphor tint per mode, scaled by each pixel's luminance.
    static const Uint8 tint[COLOR_MODE_SIZE][3] = {
        [COLOR_ORIGINAL] = {0, 0, 0}, // unused
        [COLOR_GREEN] = {0x33, 0xFF, 0x33},
        [COLOR_WHITE] = {0xFF, 0xFF, 0xFF},
    };

    int n = src->w * src->h;
    Uint32 *sp = src->pixels;
    Uint32 *dp = dst->pixels;

    for (int i = 0; i < n; i++) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(sp[i], src->format, &r, &g, &b, &a);

        if (mode != COLOR_ORIGINAL) {
            // Rec.601 luma with integer weights summing to 256.
            Uint32 lum = (77 * r + 150 * g + 29 * b) >> 8;
            r = tint[mode][0] * lum / 255;
            g = tint[mode][1] * lum / 255;
            b = tint[mode][2] * lum / 255;
        }

        dp[i] = SDL_MapRGBA(dst->format, r, g, b, a);
    }
}

// Load an image file as an RGBA32 surface. Calls die() on error.
static SDL_Surface *load_surface(char *file)
{
    char *path = path_join(TEXTURES_DIR, file);
    SDL_Surface *loaded = IMG_Load(path);
    free(path);
    if (loaded == NULL) {
        die("failed to load texture: %s", IMG_GetError());
    }

    SDL_Surface *rgba = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(loaded);
    if (rgba == NULL) {
        die("failed to convert texture: %s", SDL_GetError());
    }

    return rgba;
}

/*
 * Load texture image from file, applying the current color mode.
 * Calls die() on error. Used for transient images not affected by runtime
 * color mode toggling.
 */
SDL_Texture *texture_load(SDL_Renderer *renderer, char *file)
{
    SDL_Surface *rgba = load_surface(file);
    apply_color_mode(rgba, rgba, current_mode);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, rgba);
    SDL_FreeSurface(rgba);
    if (texture == NULL) {
        die("failed to create texture: %s", SDL_GetError());
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    return texture;
}

// Load a persistent texture, keeping its original pixels so the color mode can
// be re-applied later via texture_set_color_mode().
static SDL_Texture *load_persistent(SDL_Renderer *renderer, char *file, int idx)
{
    SDL_Surface *src = load_surface(file);
    sources[idx] = src;

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC, src->w, src->h);
    if (texture == NULL) {
        die("failed to create texture: %s", SDL_GetError());
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    SDL_Surface *tmp = SDL_CreateRGBSurfaceWithFormat(0, src->w, src->h, 32,
        SDL_PIXELFORMAT_RGBA32);
    if (tmp == NULL) {
        die("failed to create surface: %s", SDL_GetError());
    }
    apply_color_mode(tmp, src, current_mode);
    SDL_UpdateTexture(texture, NULL, tmp->pixels, tmp->pitch);
    SDL_FreeSurface(tmp);

    return texture;
}

void texture_init(SDL_Renderer *renderer)
{
    textures[TEXTURE_BRICK] = load_persistent(renderer, "brick.png", TEXTURE_BRICK);
    textures[TEXTURE_EMPTY] = NULL;
    textures[TEXTURE_GOLD] = load_persistent(renderer, "gold.png", TEXTURE_GOLD);
    textures[TEXTURE_GUARD] = load_persistent(renderer, "guard.png", TEXTURE_GUARD);
    // texture_map[TILE_HLADDER] = load_persistent(renderer, ".png", ...);
    textures[TEXTURE_HOLE] = load_persistent(renderer, "hole.png", TEXTURE_HOLE);
    textures[TEXTURE_LADDER] = load_persistent(renderer, "ladder.png", TEXTURE_LADDER);
    textures[TEXTURE_PAUSED] = load_persistent(renderer, "paused.png", TEXTURE_PAUSED);
    textures[TEXTURE_ROPE] = load_persistent(renderer, "rope.png", TEXTURE_ROPE);
    textures[TEXTURE_RUNNER] = load_persistent(renderer, "runner.png", TEXTURE_RUNNER);
    textures[TEXTURE_SOLID] = load_persistent(renderer, "solid.png", TEXTURE_SOLID);
    // texture_map[TILE_TRAP] = load_persistent(renderer, ".png", ...);
    textures[TEXTURE_GROUND] = load_persistent(renderer, "ground.png", TEXTURE_GROUND);
    textures[TEXTURE_TEXT] = load_persistent(renderer, "text.png", TEXTURE_TEXT);
}

void texture_destroy(void)
{
    for (int i = 0; i < TEXTURE_SIZE; i++) {
        SDL_DestroyTexture(textures[i]);
        textures[i] = NULL;
        if (sources[i] != NULL) {
            SDL_FreeSurface(sources[i]);
            sources[i] = NULL;
        }
    }
}

SDL_Texture *texture_get(enum texture t)
{
    return textures[t];
}

void texture_set_color_mode(enum color_mode mode)
{
    current_mode = mode;

    for (int i = 0; i < TEXTURE_SIZE; i++) {
        SDL_Surface *src = sources[i];
        if (src == NULL || textures[i] == NULL) {
            continue;
        }

        SDL_Surface *tmp = SDL_CreateRGBSurfaceWithFormat(0, src->w, src->h, 32,
            SDL_PIXELFORMAT_RGBA32);
        if (tmp == NULL) {
            die("failed to create surface: %s", SDL_GetError());
        }
        apply_color_mode(tmp, src, mode);
        SDL_UpdateTexture(textures[i], NULL, tmp->pixels, tmp->pitch);
        SDL_FreeSurface(tmp);
    }
}

enum color_mode texture_color_mode(void)
{
    return current_mode;
}

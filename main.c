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

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL2/SDL_mixer.h"
#include "exit.h"
#include "game.h"
#include "level.h"
#include "path.h"
#include "sound.h"
#include "texture.h"
#include "xmalloc.h"

#define SCREEN_WIDTH (MAP_WIDTH * TILE_MAP_WIDTH)
#define SCREEN_HEIGHT (MAP_HEIGHT * TILE_MAP_HEIGHT \
        + TILE_GROUND_HEIGHT + TILE_TEXT_HEIGHT)

#define FPS 23
#define FRAME_TIME (1000.0 / FPS)

#define PROG_NAME "loderunner-ng"

static void usage(void)
{
    fprintf(stderr, "usage: %s [-fm] [-l level] [-V volume]\n", PROG_NAME);
}

static void error(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "%s: ", PROG_NAME);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

static bool ignore_key(int key)
{
    return key != SDLK_RETURN && key != SDLK_SPACE && key != SDLK_q;
}

static bool quit_key(int key)
{
    return key == SDLK_q;
}

static int get_key(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            return event.key.keysym.sym;
        }
    }

    return 0;
}

// Render texture at the center of the screen.
static void render_texture(SDL_Renderer *renderer, SDL_Texture *t)
{
    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = 0;
    src.h = 0;
    SDL_QueryTexture(t, NULL, NULL, &src.w, &src.h);

    SDL_Rect dst;
    dst.x = (SCREEN_WIDTH - src.w) / 2;
    dst.y = (SCREEN_HEIGHT - src.h) / 2;
    dst.w = src.w;
    dst.h = src.h;

    if (SDL_RenderCopy(renderer, t, &src, &dst) < 0) {
        die("failed to render a texture: %s", SDL_GetError());
    }
}

// Load texture from file and render it at the center of the screen.
// Wait for a key to continue or exit.
// Returns true if quit game key has been pressed.
static bool display_file(SDL_Renderer *renderer, char *file)
{
    SDL_Texture *t = texture_load(renderer, file);
    int k;

    for (;;) {
        SDL_RenderClear(renderer);
        render_texture(renderer, t);
        SDL_RenderPresent(renderer);

        k = get_key();
        if (!ignore_key(k)) {
            break;
        }

        SDL_Delay(FRAME_TIME);
    }
    SDL_DestroyTexture(t);

    return quit_key(k);
}

int main(int argc, char **argv)
{
    int ch;
    int start_level = 1;
    int volume = 100;
    Uint32 wflags = 0;

    while ((ch = getopt(argc, argv, "fl:mV:")) != -1) {
        switch (ch) {
        case 'f':
            wflags |= SDL_WINDOW_FULLSCREEN;
            break;
        case 'l':
            errno = 0;
            start_level = (int) strtol(optarg, NULL, 10);
            if (errno != 0 || start_level < 1 || start_level > NLEVELS) {
                error("invalid level: %s", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'm':
            volume = 0;
            break;
        case 'V':
            errno = 0;
            volume = (int) strtol(optarg, NULL, 10);
            if (errno != 0 || volume < 0 || volume > 100) {
                error("invalid volume: %s", optarg);
                return EXIT_FAILURE;
            }
            break;
        default:
            usage();
            return EXIT_FAILURE;
        }
    }

    srandom(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        die("failed to initialize SDL: %s", SDL_GetError());
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        die("failed to initialize SDL_image: %s", SDL_GetError());
    }

    int mxflags = MIX_INIT_OGG;
    if (Mix_Init(mxflags) != mxflags) {
        die("failed to initialize SDL_mixer: %s", Mix_GetError());
    }

#ifdef SDL_AUDIODRIVER
    if (SDL_AudioInit(SDL_AUDIODRIVER) != 0) {
        die("failed to initialize audio driver %s: %s", SDL_AUDIODRIVER, Mix_GetError());
    }
#endif

    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) == -1) {
        die("failed to open audio device: %s", Mix_GetError());
    }

    SDL_Window *window = SDL_CreateWindow("Lode Runner",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, wflags);
    if (window == NULL) {
        die("failed to create SDL window: %s", SDL_GetError());
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        die("failed to initialize SDL renderer: %s", SDL_GetError());
    }

    if (SDL_RenderSetScale(renderer, 1, 1) != 0) {
        die("failed to scale renderer: %s", SDL_GetError());
    }
    if (SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255) != 0) {
        die("failed to set renderer color: %s", SDL_GetError());
    }

    texture_init(renderer);
    sound_init();
    sound_volume(volume);

    for (;;) {
        bool quit;

        quit = display_file(renderer, "start.png");
        if (quit) {
            break;
        }

        struct level *lvl = level_init(start_level);
        struct game *game = game_init(lvl);
        bool pause = false;
        bool start = false;

        double delay = 0;
        int key = 0;

        for (;;) {
            unsigned long stime = SDL_GetTicks64();
            delay += FRAME_TIME;

            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                    case SDLK_q:
                        start = true;
                        goto eog;
                    case SDLK_ESCAPE:
                    case SDLK_p:
                        pause = !pause;
                        sound_pause();
                        break;
                    default:
                        key = event.key.keysym.sym;
                        break;
                    }
                    break;
                case SDL_KEYUP:
                    if (key == event.key.keysym.sym) {
                        key = 0;
                    }
                    break;
                case SDL_QUIT:
                    quit = true;
                    goto eog;
                }
            }

            if (!pause && game_tick(game, key)) {
                if (game->won) {
                    int l = lvl->num + 1;
                    int score = game->score;

                    if (l == NLEVELS) {
                        goto eog;
                    }

                    game_destroy(game);
                    level_destroy(lvl);

                    lvl = level_init(l);
                    game = game_init(lvl);
                    game_score(game, score);
                } else {
                    goto eog;
                }
            } else {
                SDL_RenderClear(renderer);
                game_render(game, renderer);
                if (pause) {
                    render_texture(renderer, texture_get(TEXTURE_PAUSED));
                }
                SDL_RenderPresent(renderer);
            }

            if (delay > 0) {
                SDL_Delay(delay);
                delay -= (SDL_GetTicks64() - stime);
            }
        }

    eog:
        sound_stop();

        if (!start && !quit) {
            if (!game->won) {
                quit = display_file(renderer, "gameover.png");
            }
        }

        game_destroy(game);
        level_destroy(lvl);

        if (start) {
            continue;
        }
        if (quit) {
            break;
        }
    }

    texture_destroy();
    sound_destroy();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}

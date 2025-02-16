#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL2/SDL_mixer.h"
#include "exit.h"
#include "game.h"
#include "level.h"
#include "path.h"
#include "render.h"
#include "sound.h"
#include "texture.h"
#include "xmalloc.h"

#define SCREEN_WIDTH (MAP_WIDTH * TILE_MAP_WIDTH)
#define SCREEN_HEIGHT (MAP_HEIGHT * TILE_MAP_HEIGHT \
        + TILE_GROUND_HEIGHT + TILE_TEXT_HEIGHT)

#define FPS 23
#define FRAME_TIME (1000.0 / FPS)

// Render texture at the center of the screen.
static void render_texture(SDL_Renderer *renderer, char *texture)
{
    SDL_Texture *t = texture_load(renderer, texture);

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
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(t);
}

static int key_wait(void)
{
    for (;;) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                return event.key.keysym.sym;
            }
        }

        SDL_Delay(FRAME_TIME);
    }
}

static void key_wait_pause(void)
{
    while (key_wait() != SDLK_p);
}

static bool key_quit(int key)
{
    return key == SDLK_q || key == SDLK_ESCAPE;
}

int main(void)
{
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

    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) == -1) {
        die("failed to open audio device: %s", Mix_GetError());
    }

    SDL_Window *window = SDL_CreateWindow("Lode Runner",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == NULL) {
        die("failed to create SDL window: %s", SDL_GetError());
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        die("failed to initialize SDL renderer: %s", SDL_GetError());
    }

    if (SDL_RenderSetScale(renderer, 0.8, 0.8) != 0) {
        die("failed to scale renderer: %s", SDL_GetError());
    }
    if (SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255) != 0) {
        die("failed to set renderer color: %s", SDL_GetError());
    }

    /* SDL_Texture *block = texture_load(renderer, "block.png"); */
    // SDL_Texture *brick = texture_load(renderer, "brick.png");

    // blit(renderer, brick, 100, 100);

    texture_init(renderer);
    sound_init();


    /* struct tile_text *t = xmalloc(sizeof(struct tile_text)); */
    /* t->texture = texture_get(TEXTURE_TEXT); */
    /* t->ch = '5'; */
    /* t->x = 10; */
    /* t->y = 10; */
    /* t->w = TILE_TEXT_WIDTH; */
    /* t->h = TILE_TEXT_HEIGHT; */

    /* SDL_Event event; */


    for (;;) {
        SDL_RenderClear(renderer);
        render_texture(renderer, "start.png");
        if (key_quit(key_wait())) {
            break;
        }

        struct level *lvl = level_init(1);
        struct game *game = game_init(lvl);
        bool quit = false;
        bool start = false;
        bool won = false;

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
                    case SDLK_ESCAPE:
                        start = true;
                        goto eog;
                    case SDLK_p:
                        render_texture(renderer, "paused.png");
                        sound_pause();
                        key_wait_pause();
                        // Pause breaks timing calculations, start over.
                        stime = SDL_GetTicks64();
                        delay = FRAME_TIME;
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

            if (game_tick(game, key)) {
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
                // blit(renderer, brick, 100, 100);
                /* render_tile_text(renderer, t); */
                SDL_RenderPresent(renderer);
            }

            if (delay > 0) {
                SDL_Delay(delay);
                delay -= (SDL_GetTicks64() - stime);
            }
        }

    eog:
        won = game->won;

        sound_stop();
        game_destroy(game);
        level_destroy(lvl);

        if (start) {
            continue;
        }
        if (quit) {
            break;
        }
        if (!won) {
            render_texture(renderer, "gameover.png");
        }
        if (key_quit(key_wait())) {
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

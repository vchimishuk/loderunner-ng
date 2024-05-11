#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "exit.h"
#include "game.h"
#include "level.h"
#include "path.h"
#include "texture.h"
#include "tile.h"
#include "render.h"
#include "xmalloc.h"

// TODO: Add ground and status line.
#define SCREEN_WIDTH (MAP_WIDTH * TILE_MAP_WIDTH)
#define SCREEN_HEIGHT (MAP_HEIGHT * TILE_MAP_HEIGHT)

#define FPS 23
#define FRAME_TIME (1000.0 / FPS)

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        die("failed to initialize SDL: %s", SDL_GetError());
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        die("failed to initialize SDL_image: %s", SDL_GetError());
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

    // TODO: Handle errors.
    SDL_RenderSetScale(renderer, 0.8, 0.8);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);

    /* SDL_Texture *block = texture_load(renderer, "block.png"); */
    // SDL_Texture *brick = texture_load(renderer, "brick.png");

    // blit(renderer, brick, 100, 100);

    texture_init(renderer);

    struct level *lvl = load_level(1);
    struct game *game = game_init(renderer, lvl);

    /* struct tile_text *t = xmalloc(sizeof(struct tile_text)); */
    /* t->texture = texture_get(TEXTURE_TEXT); */
    /* t->ch = '5'; */
    /* t->x = 10; */
    /* t->y = 10; */
    /* t->w = TILE_TEXT_WIDTH; */
    /* t->h = TILE_TEXT_HEIGHT; */

    double delay = 0;
    SDL_Event event;
    int key = 0;
    for (;;) {
        unsigned long start = SDL_GetTicks64();

        while (SDL_PollEvent(&event)) {
            // printf("e: %d\n", event.type);
            switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_DOWN:
                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_UP:
                case SDLK_x:
                case SDLK_z:
                    key = event.key.keysym.sym;
                    break;
                case SDLK_q:
                case SDLK_ESCAPE:
                    exit(0);
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_DOWN:
                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_UP:
                case SDLK_x:
                case SDLK_z:
                    if (key == event.key.keysym.sym) {
                        key = 0;
                    }
                    break;
                }
                break;
            case SDL_QUIT:
                exit(0);
                break;
            }
        }

        SDL_RenderClear(renderer);
        if (game_tick(game, key)) {
            if (game->won) {
                // TODO: Handle last level situation.
                int l = lvl->num + 1;

                game_destroy(game);
                // TODO: Free current level. destroy_level(lvl);
                lvl = load_level(l);
                game = game_init(renderer, lvl);
            } else {
                // TODO: Display GAME OVER banner, get back to start screen.
                die("GAME OVER");
            }
        } else {
            game_render(game, renderer);
            // blit(renderer, brick, 100, 100);
            /* render_tile_text(renderer, t); */
            SDL_RenderPresent(renderer);
        }

        double left = FRAME_TIME - (SDL_GetTicks64() - start);
        if (left > 0) {
            delay += left;
            long d = (long) delay;
            delay -= d;
            SDL_Delay(d);
        }
    }

    /* game_destroy */

    texture_destroy();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}

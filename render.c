#include <SDL2/SDL.h>
#include "exit.h"
#include "render.h"

void render(SDL_Renderer *renderer, struct sprite *s, int x, int y)
{
    SDL_Rect src;
    src.x = s->x;
    src.y = s->y;
    src.w = s->w;
    src.h = s->h;
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = s->w;
    dst.h = s->h;
    if (SDL_RenderCopy(renderer, s->texture, &src, &dst) < 0) {
        die_sdl("SDL_RenderCopy");
    }
}

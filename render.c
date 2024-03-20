#include <SDL2/SDL.h>
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
    // TODO: Handle errors.
    SDL_RenderCopy(renderer, s->texture, &src, &dst);
}

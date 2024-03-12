#include <SDL2/SDL.h>
#include "render.h"

// TODO: Errors? panic?
void render(SDL_Renderer *renderer, struct tile *tile)
{
    SDL_Rect src;
    src.x = tile->tx;
    src.y = tile->ty;
    src.w = tile->tw;
    src.h = tile->th;
    SDL_Rect dst;
    dst.x = tile->x;
    dst.y = tile->y;
    dst.w = tile->tw;
    dst.h = tile->th;
    // TODO: Handle errors.
    SDL_RenderCopy(renderer, tile->texture, &src, &dst);
}


/* // TODO: Errors? panic? */
/* void blit(SDL_Renderer *renderer, SDL_Texture *texture, */
/*     SDL_Rect *src, SDL_Rect *dst) */
/* { */
/* 	/\* SDL_Rect dest; *\/ */
/* 	/\* dest.x = x; *\/ */
/* 	/\* dest.y = y; *\/ */
/* 	/\* SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h); *\/ */
/*         // TODO: Handle errors. */
/* 	SDL_RenderCopy(renderer, texture, src, dst); */
/* } */

/* /\* void blit(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y) *\/ */
/* /\* { *\/ */
/* /\* 	SDL_Rect dest; *\/ */
/* /\* 	dest.x = x; *\/ */
/* /\* 	dest.y = y; *\/ */
/* /\*         // TODO: Handle errors. *\/ */
/* /\* 	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h); *\/ */
/* /\* 	SDL_RenderCopy(renderer, texture, NULL, &dest); *\/ */
/* /\* } *\/ */

/* /\* void render_tile_ground(SDL_Renderer *renderer, struct tile_ground *tile) *\/ */
/* /\* { *\/ */
/* /\*     SDL_Rect dst; *\/ */
/* /\*     dst.x = tile->x; *\/ */
/* /\*     dst.y = tile->y; *\/ */
/* /\*     dst.w = tile->w; *\/ */
/* /\*     dst.h = tile->h; *\/ */
/* /\*     blit(renderer, tile->texture, NULL, &dst); *\/ */
/* /\* } *\/ */

/* void render_tile_map(SDL_Renderer *renderer, struct tile_map *tile) */
/* { */
/*     SDL_Rect dst; */
/*     dst.x = tile->x; */
/*     dst.y = tile->y; */
/*     dst.w = tile->w; */
/*     dst.h = tile->h; */
/*     blit(renderer, tile->texture, NULL, &dst); */
/* } */

/* void render_tile_text(SDL_Renderer *renderer, struct tile_text *tile) */
/* { */
/*     int i; */
/*     if (tile->ch >= 'A' && tile->ch <= 'Z') { */

/*     } else if (tile->ch >= '0' && tile->ch <= '9'){ */
/*         i = 50 + tile->ch - '0'; */
/*     } else { */
/*         die("TODO:"); */
/*     } */
/*     int r = i / 10; */
/*     int c = i % 10; */

/*     /\* printf("ch: %c, i: %d, r: %d, c: %d\n", tile->ch, i, r, c); *\/ */

/*     SDL_Rect src; */
/*     src.x = c * tile->w; */
/*     src.y = r * tile->h; */
/*     src.w = tile->w; */
/*     src.h = tile->h; */

/*     SDL_Rect dst; */
/*     dst.x = tile->x; */
/*     dst.y = tile->y; */
/*     dst.w = tile->w; */
/*     dst.h = tile->h; */
/*     blit(renderer, tile->texture, &src, &dst); */
/* } */

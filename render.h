#ifndef RENDER_H_
#define RENDER_H_

#include "tile.h"

void render(SDL_Renderer *renderer, struct tile *tile);
/* void blit(SDL_Renderer *renderer, SDL_Texture *texture, */
/*     SDL_Rect *src, SDL_Rect *dst); */
/* void render_tile_map(SDL_Renderer *renderer, struct tile_map *tile); */
/* void render_tile_text(SDL_Renderer *renderer, struct tile_text *tile); */

#endif /* RENDER_H_ */

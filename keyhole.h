#ifndef KEYHOLE_H_
#define KEYHOLE_H_

#include <SDL2/SDL.h>

// TODO:
#define KH_PIXEL 8
// Original game takes about 5 seconds to show the scene.
#define KH_SPEED 0.7
#define KH_SCREEN_WIDTH (40 * 28 / KH_PIXEL)
#define KH_SCREEN_HEIGHT (44 * 16 / KH_PIXEL)
#define KH_MAX_RADIUS (sqrtf(KH_SCREEN_WIDTH * KH_SCREEN_WIDTH  \
            + KH_SCREEN_HEIGHT * KH_SCREEN_HEIGHT) / 2)

void keyhole_render(SDL_Renderer *renderer, int r);

#endif /* KEYHOLE_H_ */

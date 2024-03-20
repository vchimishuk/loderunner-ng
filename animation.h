#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "texture.h"

enum animation_t {
    ANIMATION_BRICK,
    ANIMATION_CLIMB_LEFT,
    ANIMATION_CLIMB_RIGHT,
    ANIMATION_DIG_LEFT,
    ANIMATION_DIG_RIGHT,
    ANIMATION_FALL_LEFT,
    ANIMATION_FALL_RIGHT,
    ANIMATION_GROUND,
    ANIMATION_HOLE_FILL,
    ANIMATION_HOLE_LEFT,
    ANIMATION_HOLE_RIGHT,
    ANIMATION_LADDER,
    ANIMATION_LEFT,
    ANIMATION_RIGHT,
    ANIMATION_ROPE,
    ANIMATION_SOLID,
    ANIMATION_UPDOWN,
};

struct sprite {
    SDL_Texture *texture;
    int x;
    int y;
    int w;
    int h;
};

struct animation {
    struct sprite **sprites;
    struct sprite **cur;
};

struct sprite *animation_sprite_init(enum texture tx, int x, int y,
    int w, int h);
struct animation *animation_init(enum animation_t t);
bool animation_tick(struct animation *a);
void animation_reset(struct animation *a);

#endif /* ANIMATION_H_ */

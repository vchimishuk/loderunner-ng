#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "texture.h"

enum animation_t {
    ANIMATION_BRICK,
    ANIMATION_GOLD,
    ANIMATION_GROUND,
    ANIMATION_GUARD_CLIMB_LEFT,
    ANIMATION_GUARD_CLIMB_RIGHT,
    ANIMATION_GUARD_FALL_LEFT,
    ANIMATION_GUARD_FALL_RIGHT,
    ANIMATION_GUARD_LEFT,
    ANIMATION_GUARD_RIGHT,
    ANIMATION_GUARD_TRAP_LEFT,
    ANIMATION_GUARD_TRAP_RIGHT,
    ANIMATION_GUARD_UPDOWN,
    ANIMATION_HOLE_FILL,
    ANIMATION_LADDER,
    ANIMATION_ROPE,
    ANIMATION_RUNNER_CLIMB_LEFT,
    ANIMATION_RUNNER_CLIMB_RIGHT,
    ANIMATION_RUNNER_DIG_LEFT,
    ANIMATION_RUNNER_DIG_RIGHT,
    ANIMATION_RUNNER_FALL_LEFT,
    ANIMATION_RUNNER_FALL_RIGHT,
    ANIMATION_RUNNER_HOLE_LEFT,
    ANIMATION_RUNNER_HOLE_RIGHT,
    ANIMATION_RUNNER_LEFT,
    ANIMATION_RUNNER_RIGHT,
    ANIMATION_RUNNER_UPDOWN,
    ANIMATION_SOLID,
};

struct sprite {
    SDL_Texture *texture;
    int x;
    int y;
    int w;
    int h;
    // Number of frames to display current sprite for before moving
    // to the next srpite.
    int frames;
};

struct animation {
    struct sprite **sprites;
    struct sprite **cur;
    int frame;
};

struct animation *animation_init(enum animation_t t);
bool animation_tick(struct animation *a);
void animation_reset(struct animation *a);
// TODO: animation_destroy();

#endif /* ANIMATION_H_ */

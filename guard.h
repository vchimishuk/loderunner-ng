#ifndef GUARD_H_
#define GUARD_H_

#include <stdbool.h>
#include "animation.h"

enum guard_state {
    GSTATE_CLIMB_LEFT,
    GSTATE_CLIMB_RIGHT,
    GSTATE_FALL_LEFT,
    GSTATE_FALL_RIGHT,
    GSTATE_LEFT,
    GSTATE_RIGHT,
    GSTATE_STOP,
    GSTATE_UPDOWN,
};

struct guard {
    // Current X (0..MAP_WIDTH) position on the map.
    int x;
    // Current Y (0..MAP_HEIGHT) position on the map.
    int y;
    // X offset in the current map tile. When guard moves to the second
    // half of the current tile it is transfered to the next tile. To keep
    // guard on the same spot on screen tx becomes negative.
    int tx;
    // Same as tx but for vertical movement: falls and ladders.
    int ty;
    // "Move left" animation.
    struct animation *lefta;
    // "Move right" animation.
    struct animation *righta;
    // Combined "move up" and "move down" animation.
    struct animation *updowna;
    // Rope climbing left animation.
    struct animation *climblefta;
    // Rope climbing right animation.
    struct animation *climbrighta;
    // Looking left guard falling down.
    struct animation *falllefta;
    // Looking right guard falling down.
    struct animation *fallrighta;
    // Currently active animation. Points to one of animations like lefta,
    // righta, updowna, ...
    struct animation *cura;
    // Guard state defines is it moving left/right, falling, etc.
    enum guard_state state;
    // true if the guard carring a gold.
    bool gold;
};

struct guard *guard_init();
void guard_reset(struct guard *guard);
struct animation *guard_state_animation(struct guard *g, enum guard_state s);

#endif /* GUARD_H_ */

#ifndef RUNNER_H_
#define RUNNER_H_

enum runner_state {
    RSTATE_CLIMB_LEFT,
    RSTATE_CLIMB_RIGHT,
    RSTATE_DIG_LEFT,
    RSTATE_DIG_RIGHT,
    RSTATE_FALL_LEFT,
    RSTATE_FALL_RIGHT,
    RSTATE_LEFT,
    RSTATE_RIGHT,
    RSTATE_STOP,
    RSTATE_UPDOWN,
};

struct runner {
    // Start X (0..MAP_WIDTH) position on the map.
    int sx;
    // Start Y (0..MAP_HEIGHT) position on the map.
    int sy;
    // Current X (0..MAP_WIDTH) position on the map.
    int x;
    // Current Y (0..MAP_HEIGHT) position on the map.
    int y;
    // X offset in the current map tile. When runner moves to the second
    // half of the current tile it is transfered to the next tile. To keep
    // runner on the same spot on screen tx becomes negative.
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
    // Runner digging hole on the left of the runner.
    struct animation *diglefta;
    // Runner digging hole on the right of the runner.
    struct animation *digrighta;
    // Looking left runner falling down.
    struct animation *falllefta;
    // Looking right runner falling down.
    struct animation *fallrighta;
    // Hole on the left of the runner.
    struct animation *holelefta;
    // Hole on the right of the runner.
    struct animation *holerighta;
    // Currently active animation. Points to one of animations like lefta,
    // righta, updowna, ...
    struct animation *cura;
    // Runner state defines is it moving left/right, falling, etc.
    enum runner_state state;
};

struct runner *runner_init();
void runner_reset(struct runner *r);
struct animation *runner_state_animation(struct runner *r, enum runner_state s);

#endif /* RUNNER_H_ */

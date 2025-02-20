// Copyright 2024-2025 Viacheslav Chimishuk <vchimishuk@yandex.ru>
//
// This file is part of loderunner-ng.
//
// loderunner-ng is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// loderunner-ng is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with loderunner-ng. If not, see <http://www.gnu.org/licenses/>.

#ifndef GUARD_H_
#define GUARD_H_

#include <stdbool.h>
#include "animation.h"

enum guard_state {
    GSTATE_CLIMB_LEFT,
    GSTATE_CLIMB_OUT,
    GSTATE_CLIMB_RIGHT,
    GSTATE_FALL_LEFT,
    GSTATE_FALL_RIGHT,
    GSTATE_LEFT,
    GSTATE_REBORN,
    GSTATE_RIGHT,
    GSTATE_STOP,
    GSTATE_TRAP_LEFT,
    GSTATE_TRAP_RIGHT,
    GSTATE_UPDOWN,
};

struct guard {
    // Start X position.
    int sx;
    // Start Y position.
    int sy;
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
    // Looking left shaking (before getting out from the hole) animation.
    struct animation *traplefta;
    // Looking right shaking (before getting out from the hole) animation.
    struct animation *traprighta;
    // Guard reborn animation. After reborn animation completed guard moves to
    // falling right state.
    struct animation *reborna;
    // Currently active animation. Points to one of animations like lefta,
    // righta, updowna, ...
    struct animation *cura;
    // Guard state defines is it moving left/right, falling, etc.
    enum guard_state state;
    // true if the guard is still climbing out of the hole and still located
    // over it. Used to prevent falling back into the hole we have just climbed
    // out from.
    bool hole;
    // To prevent falling into the hole when crossing it horizontally we store Y
    // coordinate of hole we are falling into. -1 when no falling is active.
    int holey;
    // Gold guard is holding if any.
    struct gold *gold;
    // When guard picks up a gold a random number is generated. As guard moves
    // during the game this counter is decremented every time guard moves to the
    // next map tile. Gold is dropped when 0 is reached.
    int goldholds;
};

struct guard *guard_init(int x, int y);
void guard_destroy(struct guard *g);
void guard_reset(struct guard *g);
struct animation *guard_state_animation(struct guard *g, enum guard_state s);

#endif /* GUARD_H_ */

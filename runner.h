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
    // Number of picked up gold items. If this number is equals to the number
    // in `struct game` game is won.
    int ngold;
};

struct runner *runner_init(void);
void runner_destroy(struct runner *r);
void runner_reset(struct runner *r);
struct animation *runner_state_animation(struct runner *r, enum runner_state s);

#endif /* RUNNER_H_ */

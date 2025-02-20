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

#ifndef GOLD_H_
#define GOLD_H_

#include "animation.h"

struct gold {
    int sx;
    int sy;
    int x;
    int y;
    // Flag indicating whether the gold should be rendered or not.
    // When runner or guard picks the gold up we simply hide it
    // and display it back when it is dropped.
    bool visible;
    // When guard holding gold dies he drops gold on the ground. However,
    // it can be a situation when there is not place to drop gold to
    // (for example guard is walled deep into a wall). In this case gold
    // he holds become completely lost and cannot be picked up bu the
    // runner any more.
    bool lost;
    struct animation *animation;
};

struct gold *gold_init(int x, int y);
void gold_destroy(struct gold *gold);
void gold_reset(struct gold *gold);
struct gold *gold_get(struct game *g, int x, int y);
struct gold *gold_pickup(struct game *g, int x, int y, int tx, int ty);
void gold_drop(struct gold *g, int x, int y);
void gold_lose(struct gold *g);

#endif /* GOLD_H_ */

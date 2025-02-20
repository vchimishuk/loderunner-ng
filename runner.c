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

#include "animation.h"
#include "exit.h"
#include "runner.h"
#include "texture.h"
#include "xmalloc.h"

struct runner *runner_init(void)
{
    struct runner *r = xmalloc(sizeof(struct runner));
    r->sx = 0;
    r->sy = 0;
    r->lefta = animation_init(ANIMATION_RUNNER_LEFT);
    r->righta = animation_init(ANIMATION_RUNNER_RIGHT);
    r->updowna = animation_init(ANIMATION_RUNNER_UPDOWN);
    r->climblefta = animation_init(ANIMATION_RUNNER_CLIMB_LEFT);
    r->climbrighta = animation_init(ANIMATION_RUNNER_CLIMB_RIGHT);
    r->diglefta = animation_init(ANIMATION_RUNNER_DIG_LEFT);
    r->digrighta = animation_init(ANIMATION_RUNNER_DIG_RIGHT);
    r->falllefta = animation_init(ANIMATION_RUNNER_FALL_LEFT);
    r->fallrighta = animation_init(ANIMATION_RUNNER_FALL_RIGHT);
    r->holelefta = animation_init(ANIMATION_RUNNER_HOLE_LEFT);
    r->holerighta = animation_init(ANIMATION_RUNNER_HOLE_RIGHT);

    runner_reset(r);

    return r;
}

void runner_destroy(struct runner *r)
{
    animation_destroy(r->lefta);
    animation_destroy(r->righta);
    animation_destroy(r->updowna);
    animation_destroy(r->climblefta);
    animation_destroy(r->climbrighta);
    animation_destroy(r->diglefta);
    animation_destroy(r->digrighta);
    animation_destroy(r->falllefta);
    animation_destroy(r->fallrighta);
    animation_destroy(r->holelefta);
    animation_destroy(r->holerighta);
    free(r);
}

void runner_reset(struct runner *r)
{
    r->x = r->sx;
    r->y = r->sy;
    r->tx = 0;
    r->ty = 0;
    r->cura = r->righta;
    r->state = RSTATE_RIGHT;
    r->ngold = 0;
}

struct animation *runner_state_animation(struct runner *r, enum runner_state s)
{
    switch (s) {
    case RSTATE_DIG_LEFT:
        return r->diglefta;
    case RSTATE_CLIMB_LEFT:
        return r->climblefta;
    case RSTATE_CLIMB_RIGHT:
        return r->climbrighta;
    case RSTATE_DIG_RIGHT:
        return r->digrighta;
    case RSTATE_FALL_LEFT:
        return r->falllefta;
    case RSTATE_FALL_RIGHT:
        return r->fallrighta;
    case RSTATE_LEFT:
        return r->lefta;
    case RSTATE_RIGHT:
        return r->righta;
    case RSTATE_UPDOWN:
        return r->updowna;
    default:
        die("illegal state");
    }
}

#include <stdlib.h>
#include "ai.h"
#include "exit.h"
#include "guard.h"
#include "level.h"
#include "phys.h"
#include "tile.h"

// TODO: Should we join ai.c and game.c?

// TODO: Looks like this is the max number of guards possible.
//       Update another define.
#define MP_NGUARDS 12
#define MP_NMOVES 6

// Algorithm traces possible routes and calculate rating (score) for every
// route. Lower rating is better.
// Maximum rating constant works like a "disabled" value -- no route here.
#define RATING_MAX 255
// We want routes leading to level above the runner beat routes
// below him. To make it works we use this trick with rating offset.
#define RATING_BASE_BELLOW 200
#define RATING_BASE_ABOVE 100

static int move_policy[MP_NGUARDS][MP_NMOVES] = {
    {0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1},
    {1, 2, 1, 1, 2, 1},
    {1, 2, 2, 1, 2, 2},
    {2, 2, 2, 2, 2, 2},
    {2, 2, 3, 2, 2, 3},
    {2, 3, 3, 2, 3, 3},
    {3, 3, 3, 3, 3, 3},
    {3, 3, 4, 3, 3, 4},
    {3, 4, 4, 3, 4, 4},
    {4, 4, 4, 4, 4, 4},
};

// Direction to move the guard to.
enum dir {
    DIR_DOWN,
    DIR_FALL,
    DIR_LEFT,
    DIR_NONE,
    DIR_RIGHT,
    DIR_UP,
};

// If guard and the runner on the same level (map row) guard moves
// directly toward the runner.
static enum dir ai_scan_level(struct game *game, struct guard *guard)
{
    int rx = game->runner->x;
    int ry = game->runner->y;
    int gx = guard->x;
    int gy = guard->y;

    if (ry != gy) {
        return DIR_NONE;
    }

    while (gx != rx) {
        enum map_tile_t lvl = game->map[gy][gx]->baset;
        enum map_tile_t nextlvl;
        if (gy < MAP_HEIGHT) {
            nextlvl = game->map[gy + 1][gx]->baset;
        } else {
            nextlvl = MAP_TILE_SOLID;
        }

        // Check if we can walk on the next level or use ladder or rope on
        // the current level to avoid falling.
        //
        // TODO: Handle also situations when there is a hole with a guard
        //       trapped in that hole. In this case we can move on his head.
        //       Check level 43.
        //
        // TODO: Check nextlvl == MAP_TILE_ROPE for the level 92?
        if (lvl == MAP_TILE_LADDER || lvl == MAP_TILE_ROPE
            || nextlvl == MAP_TILE_SOLID || nextlvl == MAP_TILE_LADDER
            || nextlvl == MAP_TILE_BRICK) {
            if (gx < rx) {
                gx++;
            } else {
                gx--;
            }
        } else {
            break;
        }
    }

    if (gx == rx) {
        if (guard->x < rx) {
            return DIR_RIGHT;
        } else if (guard->x > rx) {
            return DIR_LEFT;
        } else {
            if (guard->tx < game->runner->tx) {
                return DIR_RIGHT;
            } else {
                return DIR_LEFT;
            }
        }
    }

    // Route tracing for the current level has not succeeded,
    // try other directions.
    return DIR_NONE;
}

// Scan downward direction.
// See also ai_scan_up() implementation.
static int ai_scan_down(struct game *game, int x, int y, int startx)
{
    // Return "no route" if cannot move down.
    if (y < MAP_HEIGHT - 1
        && (is_tile(game, x, y + 1, MAP_TILE_BRICK)
            || is_tile(game, x, y + 1, MAP_TILE_SOLID))) {
        return RATING_MAX;
    }

    // Until we haven't reached the ground.
    while (y < MAP_HEIGHT && !is_tile(game, x, y + 1, MAP_TILE_BRICK)
        && !is_tile(game, x, y + 1, MAP_TILE_SOLID)) {
        // Try to trace left and right if we can (not in a freefall mode).
        if (!is_tile(game, x, y, MAP_TILE_EMPTY)) {
            // Check if we can turn left.
            if (x > 0) {
                // Check if we can potentially move left.
                // This check looks strange, as we do not check if there is an empty
                // tile at x-1:y where the guard can move into. But this is how it
                // is implemented in the original code.
                if (is_tile(game, x - 1, y + 1, MAP_TILE_BRICK)
                    || is_tile(game, x - 1, y + 1, MAP_TILE_SOLID)
                    || is_tile(game, x - 1, y + 1, MAP_TILE_LADDER)
                    || is_tile(game, x - 1, y, MAP_TILE_ROPE)) {
                    // No need to keep moving down if we are already
                    // below the runner.
                    if (y >= game->runner->y) {
                        break;
                    }
                }
            }
            // The same check for right.
            if (x < MAP_WIDTH - 1) {
                if (is_tile(game, x + 1, y + 1, MAP_TILE_BRICK)
                    || is_tile(game, x + 1, y + 1, MAP_TILE_SOLID)
                    || is_tile(game, x + 1, y + 1, MAP_TILE_LADDER)
                    || is_tile(game, x + 1, y, MAP_TILE_ROPE)) {
                    if (y >= game->runner->y) {
                        break;
                    }
                }
            }
        }
        y++;
    }

    if (y == game->runner->y) {
        return abs(startx - x);
    } else if (y > game->runner->y) {
        return RATING_BASE_BELLOW + (y - game->runner->y);
    } else {
        return RATING_BASE_ABOVE + (game->runner->y - y);
    }
}

// Scan upward direction.
// Our main goal here is to move above the runner. The first level above
// the runner we can reach going up wins.
static int ai_scan_up(struct game *game, int x, int y, int startx)
{
    // We cannot move up without ladder for sure.
    if (!is_tile(game, x, y, MAP_TILE_LADDER)) {
        return RATING_MAX;
    }

    while (y > 0 && is_tile(game, x, y, MAP_TILE_LADDER)) {
        y--;

        if (x > 0) {
            // Check if we can potentially move left.
            // This check looks strange, as we do not check if there is an empty
            // tile at x-1:y where the guard can move into. But this is how it
            // is implemented in the original code.
            if (is_tile(game, x - 1, y + 1, MAP_TILE_BRICK)
                || is_tile(game, x - 1, y + 1, MAP_TILE_SOLID)
                || is_tile(game, x - 1, y + 1, MAP_TILE_LADDER)
                || is_tile(game, x - 1, y, MAP_TILE_ROPE)) {
                // No need to keep moving up if we are already above the runner.
                if (y <= game->runner->y) {
                    break;
                }
            }
        }
        // Perform the same logic for the right edge of the ladder.
        if (x < MAP_WIDTH) {
            if (is_tile(game, x + 1, y + 1, MAP_TILE_BRICK)
                || is_tile(game, x + 1, y + 1, MAP_TILE_SOLID)
                || is_tile(game, x + 1, y + 1, MAP_TILE_LADDER)
                || is_tile(game, x + 1, y, MAP_TILE_ROPE)) {
                if (y <= game->runner->y) {
                    break;
                }
            }
        }
    }

    if (y == game->runner->y) {
        return abs(startx - x);
    } else if (y > game->runner->y) {
        return RATING_BASE_BELLOW + (y - game->runner->y);
    } else {
        return RATING_BASE_ABOVE + (game->runner->y - y);
    }
}

// Scan horizontally right or left.
// Looking horizontally we execute up and down scan for the every step we
// can perform left or right depends on the `left` flag. Up and down branches
// are ranged by rating, more distant route wins between two routes with the
// same rating.
static int ai_scan_horizontal(struct game *game, int x, int y, bool left)
{
    int rating = RATING_MAX;
    int startx = x;
    int dx = left ? -1 : 1;

    for (;;) {
        // Check if we have reached edge of the screen.
        if (left && x == 0) {
            break;
        } else if (!left && x == MAP_WIDTH - 1) {
            break;
        }

        if (is_tile(game, x + dx, y, MAP_TILE_BRICK)
            || is_tile(game, x + dx, y, MAP_TILE_SOLID)) {
            // We have reached a wall.
            break;
        }

        // Can climb left despite what is under the feet.
        bool climb = is_tile(game, x + dx, y, MAP_TILE_LADDER)
            || is_tile(game, x + dx, y, MAP_TILE_ROPE);
        // Can walk over the solid ground.
        bool walk = (y < MAP_HEIGHT - 1)
            && (is_tile(game, x + dx, y + 1, MAP_TILE_BRICK)
                || is_tile(game, x + dx, y + 1, MAP_TILE_SOLID)
                || is_tile(game, x + dx, y + 1, MAP_TILE_LADDER));
        // Reached the bottom of the map so can walk on its edge.
        bool ground = (y == MAP_HEIGHT - 1);

        x += dx;

        // Usually of the two routes with the same rating the first traced wins
        // but for right and left tracing original code uses another strategy.
        // It traces starting from the farthest point and moves to the runner.
        // Since I start from the runner and move left we need this rating
        // comparison inversion.
        // Again, I'm not this is really matters because after runner moves
        // using this route it will be recalculated and up/down lookup goes
        // first. As a result, probably, this can be simplified.
        int r = ai_scan_down(game, x, y, startx);
        int q = ai_scan_up(game, x, y, startx);
        if (q < r) {
            r = q;
        }
        if (r <= rating) {
            rating = r;
        }

        if (!climb && !walk && !ground) {
            // Falling down.
            // For me it looks like this complex check be replaced with
            // a simple falling down check. Strange that the original code
            // doesn't do that. Am I missing something?
            break;
        }
    }

    return rating;
}

static bool ai_falling(struct game *game, struct guard *guard)
{
    int x = guard->x;
    int y = guard->y;
    int ty = guard->ty;

    if (is_tile(game, x, y, MAP_TILE_LADDER)
        || is_tile(game, x, y, MAP_TILE_LADDER)) {
        return false;
    }

    // TODO: Support walking on other guard's head.
    if (ty < 0
        || (y < MAP_HEIGHT - 1
            && !is_tile(game, x, y + 1, MAP_TILE_BRICK)
            && !is_tile(game, x, y + 1, MAP_TILE_SOLID)
            && !is_tile(game, x, y + 1, MAP_TILE_LADDER))) {
        return true;
    }

    return false;
}

// Guard's AI is completely copied from Simon Hung's LodeRunner TotalRecall
// implementation. See https://github.com/SimonHung/LodeRunner_TotalRecall
// Code understanding was easy with help of great TotalRecall's source code
// overview from Ahab which is available at
// https://datadrivengamer.blogspot.com/2023/01/championship-lode-runner-guard.html
//
// Look for the direction to move the guard.
// General AI algorithm works as follows.
//  * If guard and the runner on the same level (map row) then guard tries
//    to move directly towards the runner if he can. However, rules are a
//    little bit tricky: walls and holes dug by the runner are ignore during
//    the trace.
//    If guard and the runner not on the same level next directions are traced.
//    All four directions are traced and rating for each potential route is
//    calculated. Route with the lowest rating wins.
//  * Trace down path: either guard can freefall or climb using a ladder.
//    Every time try to turn left or right as close to the runner's level as
//    possible.
//  * Trace up path: the same logic as tracing down actually.
//  * Trace left path: scan down and up for every step we can perform
//    moving left.
//  * Trace right path: the same logic as tracing left actually.
static enum dir ai_scan(struct game *game, struct guard *guard)
{
    if (ai_falling(game, guard)) {
        return DIR_FALL;
    }

    // Move towards the runner on the same level if we can.
    enum dir d = ai_scan_level(game, guard);
    if (d != DIR_NONE) {
        return d;
    }

    int score = RATING_MAX;
    int s = ai_scan_down(game, guard->x, guard->y, guard->x);
    if (s < score) {
        score = s;
        d = DIR_DOWN;
    }
    s = ai_scan_up(game, guard->x, guard->y, guard->x);
    if (s < score) {
        score = s;
        d = DIR_UP;
    }
    s = ai_scan_horizontal(game, guard->x, guard->y, true);
    if (s < score) {
        score = s;
        d = DIR_LEFT;
    }
    s = ai_scan_horizontal(game, guard->x, guard->y, false);
    if (s < score) {
        score = s;
        d = DIR_RIGHT;
    }

    return d;
}

// Make guard to make a single step in the calculated direction if we can.
static void ai_move_guard(struct game *game, struct guard *guard, enum dir d)
{
    enum guard_state state = guard->state;
    bool move = false;
    int x = guard->x;
    int y = guard->y;
    int tx = guard->tx;
    int ty = guard->ty;

    switch (d) {
    case DIR_DOWN:
        ty += MOVE_DY;
        tx = 0;
        if (ty > TILE_MAP_HEIGHT / 2) {
            y += 1;
            ty -= TILE_MAP_HEIGHT;
        }
        if (ty >= 0 && !can_move(game, x, y + 1)) {
            move = false;
        } else {
            if (is_tile(game, x, y, MAP_TILE_ROPE)
                && !is_tile(game, x, y + 1, MAP_TILE_LADDER)) {
                if (state == GSTATE_CLIMB_RIGHT) {
                    state = GSTATE_FALL_RIGHT;
                } else {
                    state = GSTATE_FALL_LEFT;
                }
            } else {
                state = GSTATE_UPDOWN;
            }
            move = true;
        }
        break;
    case DIR_FALL:
        ty += MOVE_DY;
        tx = 0;
        if (ty > TILE_MAP_HEIGHT / 2) {
            y += 1;
            ty -= TILE_MAP_HEIGHT;
        }
        if (ty >= 0 && !can_move(game, x, y + 1)) {
            ty = 0;
        }
        if (state == GSTATE_LEFT || state == GSTATE_CLIMB_LEFT) {
            state = GSTATE_FALL_LEFT;
        } else if (state == GSTATE_RIGHT || state == GSTATE_CLIMB_RIGHT) {
            state = GSTATE_FALL_RIGHT;
        }
        move = true;
        break;
    case DIR_LEFT:
        tx -= MOVE_DX;
        ty = 0;
        if (tx < -(TILE_MAP_WIDTH / 2)) {
            x -= 1;
            tx += TILE_MAP_WIDTH;
        }
        if (tx < 0 && !can_move(game, x - 1, y)) {
            move = false;
        } else {
            if (is_tile(game, x, y, MAP_TILE_ROPE)) {
                state = GSTATE_CLIMB_LEFT;
            } else {
                state = GSTATE_LEFT;
            }
            move = true;
        }
        break;
    case DIR_NONE:
        break;
    case DIR_RIGHT:
        tx += MOVE_DX;
        ty = 0;
        if (tx > TILE_MAP_WIDTH / 2) {
            x += 1;
            tx -= TILE_MAP_WIDTH;
        }
        if (tx > 0 && !can_move(game, x + 1, y)) {
            move = false;
        } else {
            if (is_tile(game, x, y, MAP_TILE_ROPE)) {
                state = GSTATE_CLIMB_RIGHT;
            } else {
                state = GSTATE_RIGHT;
            }
            move = true;
        }
        break;
    case DIR_UP:
        ty -= MOVE_DY;
        tx = 0;
        if (ty < -(TILE_MAP_HEIGHT / 2)) {
            y -= 1;
            ty += TILE_MAP_HEIGHT;
        }
        bool onladder = is_tile(game, x, y, MAP_TILE_LADDER);
        if (ty < 0 && (!onladder || !can_move(game, x, y - 1))) {
            move = false;
        } else {
            state = GSTATE_UPDOWN;
            move = true;
        }
        break;
    default:
        die("illegal state");
    }

    if (move) {
        guard->x = x;
        guard->y = y;
        guard->tx = tx;
        guard->ty = ty;
        animation_tick(guard->cura);
    }
    if (guard->state != state) {
        // When stopped keep current animation.
        if (state != GSTATE_STOP) {
            guard->cura = guard_state_animation(guard, state);
            animation_reset(guard->cura);
        }
        guard->state = state;
    }
}

// Callback to move guards.
// Called on every game loop tick, calculates direction to move for every
// guard and make the move. On every call a few guards are moved depends on
// the policy defined by move_policy table.
void ai_tick(struct game *game)
{
    static int imoves = MP_NMOVES;
    static int iguard = 0;

    if (++imoves >= MP_NMOVES) {
        imoves = 0;
    }

    int moves = move_policy[game->nguards][imoves];
    while (moves-- > 0) {
        if (++iguard >= game->nguards) {
            iguard = 0;
        }

        struct guard *g = game->guards[iguard];
        // if (can_move(g)) {
        //     continue;
        // }

        enum dir d = ai_scan(game, g);
        ai_move_guard(game, g, d);
    }
}

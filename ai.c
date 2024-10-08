#include <stdlib.h>
#include "ai.h"
#include "exit.h"
#include "gold.h"
#include "guard.h"
#include "level.h"
#include "phys.h"
#include "tile.h"

// TODO: Should we join ai.c and guard.c.

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

// Return guard at x:y coordinate if there is any.
static struct guard *guard_at_point(struct game *g, int x, int y)
{
    for (int i = 0; i < g->nguards; i++) {
        if (g->guards[i]->x == x && g->guards[i]->y == y) {
            return g->guards[i];
        }
    }

    return NULL;
}

// Return random X coordinate to reborn guard at.
static int ai_rand_rebornx()
{
    static int row[MAP_WIDTH - 1];
    static int idx = MAP_WIDTH;

    if (idx >= MAP_WIDTH) {
        for (int i = 0; i < MAP_WIDTH; i++) {
            row[i] = i;
        }
        for (int i = 0; i < MAP_WIDTH; i++) {
            int j = (int) random() % MAP_WIDTH;

            int t = row[i];
            row[i] = row[j];
            row[j] = t;
        }
        idx = 0;
    }

    return row[idx++];
}

static int ai_rand_goldholds()
{
    return (random() % 26) + 11; // 11..36
}

// Try to drop gold if it is time.
// Every time guard moves to the new map tile gold holding counter
// is decremented. When counter reaches 0 gold is dropped.
static void ai_drop_gold(struct game *game, struct guard *guard)
{
    int x = guard->x;
    int y = guard->y;

    if (guard->goldholds < 0) {
        guard->goldholds++;
    } else if (guard->goldholds == 0
        && guard->gold != NULL
        && is_tile(game, x, y, MAP_TILE_EMPTY)
        && ((y == MAP_HEIGHT - 1)
            || (is_tile(game, x, y + 1, MAP_TILE_BRICK)
                || is_tile(game, x, y + 1, MAP_TILE_SOLID)
                || is_tile(game, x, y + 1, MAP_TILE_LADDER)))) {
        gold_drop(guard->gold, x, y);
        guard->gold = NULL;
        // Set gold holding counter to -1 to prevent picking up the gold we have
        // just dropped before moving to the next tile.
        guard->goldholds = -1;
    } else if (guard->goldholds > 0) {
        guard->goldholds--;
    }
}

// Drop gold when trapped. Discard gold if it is not possible to drp the gold.
static void ai_drop_gold_trapped(struct game *game, struct guard *guard)
{
    if (guard->gold == NULL) {
        return;
    }

    int x = guard->x;
    int y = guard->y;
    if (is_tile(game, x, y - 1, MAP_TILE_EMPTY)) {
        gold_drop(guard->gold, x, y - 1);
    } else {
        game_discard_gold(game, guard->gold);
    }

    guard->gold = NULL;
    // Set goldholds small enough to prevent guard from picking up the gold back
    // he just dropped.
    guard->goldholds = -2;
}

// Return true if tested map tile acts like a hole dug by the runner.
static bool ai_hole(struct game *g, int x, int y)
{
    return g->map[y][x]->curt == MAP_TILE_EMPTY
        && g->map[y][x]->baset == MAP_TILE_BRICK;
}

// Check if tile at x:y coordinates has requested type and ignore holes dug
// by the runner. Scanning doesn't treat holes as an empty space but keep seeing
// them as bricks. However, since same-level scanning ignores bricks it ignores
// holes as well.
bool is_tilenh(struct game *game, int x, int y, enum map_tile_t t)
{
    if (t == MAP_TILE_BRICK) {
        return is_tile(game, x, y, t) || ai_hole(game, x, y);
    }

    return is_tile(game, x, y, t);
}

// Return true if guard is looking right.
static bool ai_looking_right(enum guard_state s)
{
    return s == GSTATE_CLIMB_RIGHT
        || s == GSTATE_FALL_RIGHT
        || s == GSTATE_RIGHT;
}

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
        && (is_tilenh(game, x, y + 1, MAP_TILE_BRICK)
            || is_tilenh(game, x, y + 1, MAP_TILE_SOLID))) {
        return RATING_MAX;
    }

    // Until we haven't reached the ground.
    while (y < MAP_HEIGHT && !is_tilenh(game, x, y + 1, MAP_TILE_BRICK)
        && !is_tilenh(game, x, y + 1, MAP_TILE_SOLID)) {
        // Try to trace left and right if we can (not in a freefall mode).
        if (!is_tilenh(game, x, y, MAP_TILE_EMPTY)) {
            // Check if we can turn left.
            if (x > 0) {
                // Check if we can potentially move left.
                // This check looks strange, as we do not check if there is an empty
                // tile at x-1:y where the guard can move into. But this is how it
                // is implemented in the original code.
                if (is_tilenh(game, x - 1, y + 1, MAP_TILE_BRICK)
                    || is_tilenh(game, x - 1, y + 1, MAP_TILE_SOLID)
                    || is_tilenh(game, x - 1, y + 1, MAP_TILE_LADDER)
                    || is_tilenh(game, x - 1, y, MAP_TILE_ROPE)) {
                    // No need to keep moving down if we are already
                    // below the runner.
                    if (y >= game->runner->y) {
                        break;
                    }
                }
            }
            // The same check for right.
            if (x < MAP_WIDTH - 1) {
                if (is_tilenh(game, x + 1, y + 1, MAP_TILE_BRICK)
                    || is_tilenh(game, x + 1, y + 1, MAP_TILE_SOLID)
                    || is_tilenh(game, x + 1, y + 1, MAP_TILE_LADDER)
                    || is_tilenh(game, x + 1, y, MAP_TILE_ROPE)) {
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
    if (!is_tilenh(game, x, y, MAP_TILE_LADDER)) {
        return RATING_MAX;
    }

    while (y > 0 && is_tilenh(game, x, y, MAP_TILE_LADDER)) {
        y--;

        if (x > 0) {
            // Check if we can potentially move left.
            // This check looks strange, as we do not check if there is an empty
            // tile at x-1:y where the guard can move into. But this is how it
            // is implemented in the original code.
            if (is_tilenh(game, x - 1, y + 1, MAP_TILE_BRICK)
                || is_tilenh(game, x - 1, y + 1, MAP_TILE_SOLID)
                || is_tilenh(game, x - 1, y + 1, MAP_TILE_LADDER)
                || is_tilenh(game, x - 1, y, MAP_TILE_ROPE)) {
                // No need to keep moving up if we are already above the runner.
                if (y <= game->runner->y) {
                    break;
                }
            }
        }
        // Perform the same logic for the right edge of the ladder.
        if (x < MAP_WIDTH) {
            if (is_tilenh(game, x + 1, y + 1, MAP_TILE_BRICK)
                || is_tilenh(game, x + 1, y + 1, MAP_TILE_SOLID)
                || is_tilenh(game, x + 1, y + 1, MAP_TILE_LADDER)
                || is_tilenh(game, x + 1, y, MAP_TILE_ROPE)) {
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

        if (is_tilenh(game, x + dx, y, MAP_TILE_BRICK)
            || is_tilenh(game, x + dx, y, MAP_TILE_SOLID)) {
            // We have reached a wall.
            break;
        }

        // Can climb left despite what is under the feet.
        bool climb = is_tilenh(game, x + dx, y, MAP_TILE_LADDER)
            || is_tilenh(game, x + dx, y, MAP_TILE_ROPE);
        // Can walk over the solid ground.
        bool walk = (y == MAP_HEIGHT - 1)
            || (is_tilenh(game, x + dx, y + 1, MAP_TILE_BRICK)
                || is_tilenh(game, x + dx, y + 1, MAP_TILE_SOLID)
                || is_tilenh(game, x + dx, y + 1, MAP_TILE_LADDER));

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

        if (!climb && !walk) {
            // Falling down.
            // For me it looks like this complex check be replaced with
            // a simple falling down check. Strange that the original code
            // doesn't do that. Am I missing something?
            break;
        }
    }

    return rating;
}

// Check if the guard must freefall down into a hole or over regular empty tile.
static bool ai_falling(struct game *game, struct guard *guard)
{
    int x = guard->x;
    int y = guard->y;
    int ty = guard->ty;
    int hdy = MOVE_DY / 2;

    if (is_tile(game, x, y, MAP_TILE_LADDER)
        || (is_tile(game, x, y, MAP_TILE_ROPE) && (ty > -hdy && ty <= hdy))) {
        return false;
    }

    if (ty < 0
        || (y < MAP_HEIGHT - 1
            && !is_tile(game, x, y + 1, MAP_TILE_BRICK)
            && !is_tile(game, x, y + 1, MAP_TILE_SOLID)
            && !is_tile(game, x, y + 1, MAP_TILE_LADDER)
            && !guard_at_point(game, x, y + 1))) {
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
    // Avoid falling back to the hole we have just climbed out from by
    // disabling all movement or falling down.
    bool no_down = guard->hole;

    if (guard->state == GSTATE_CLIMB_OUT) {
        return DIR_UP;
    }
    if (!no_down && ai_falling(game, guard)) {
        return DIR_FALL;
    }

    // Move towards the runner on the same level if we can.
    enum dir d = ai_scan_level(game, guard);
    if (d != DIR_NONE) {
        return d;
    }

    int score = RATING_MAX;
    int s;
    if (!no_down) {
        s = ai_scan_down(game, guard->x, guard->y, guard->x);
        if (s < score) {
            score = s;
            d = DIR_DOWN;
        }
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
            ai_drop_gold(game, guard);
        }
        if (ty >= 0 && !can_move(game, x, y + 1)) {
            move = false;
        } else {
            if (!is_tile(game, x, y, MAP_TILE_LADDER) &&
                !is_tile(game, x, y + 1, MAP_TILE_LADDER)) {
                if (ai_looking_right(state)) {
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
            ai_drop_gold(game, guard);
        }
        bool trapped = false;

        if (ty < 0 && ai_hole(game, x, y)) {
            guard->holey = y;
        }
        if (ty >= 0) {
            if (ai_hole(game, x, y) && guard->holey == y) {
                trapped = true;
                guard->hole = true;
                ty = 0;
                ai_drop_gold_trapped(game, guard);
            } else if (!can_move(game, x, y + 1)
                && !is_tile(game, x, y + 1, MAP_TILE_FALSE)) {
                ty = 0;
            }
        }
        if (ai_looking_right(state)) {
            state = trapped ? GSTATE_TRAP_RIGHT : GSTATE_FALL_RIGHT;
        } else {
            state = trapped ? GSTATE_TRAP_LEFT : GSTATE_FALL_LEFT;
        }
        move = true;
        break;
    case DIR_LEFT:
        tx -= MOVE_DX;
        ty = 0;
        if (tx < -(TILE_MAP_WIDTH / 2)) {
            x -= 1;
            tx += TILE_MAP_WIDTH;
            guard->hole = false;
            guard->holey = -1;
            ai_drop_gold(game, guard);
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
            guard->hole = false;
            guard->holey = -1;
            ai_drop_gold(game, guard);
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
            ai_drop_gold(game, guard);
        }

        bool onladder = is_tile(game, x, y, MAP_TILE_LADDER);
        bool climb_out = guard->state == GSTATE_CLIMB_OUT;

        if (climb_out) {
            if (ai_hole(game, x, y) && guard->holey == y) {
                if (can_move(game, x, y - 1)) {
                    // Climb up from the hole if we can.
                    move = true;
                }
            } else {
                // We are over the hole now, let's move left or right now.
                state = GSTATE_UPDOWN;
                move = true;
            }
        } else if (ty < 0 && (!onladder || !can_move(game, x, y - 1))) {
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

// Set guard into reborn state after he died immured in the wall.
void ai_reborn(struct game *game, struct guard *guard)
{
    int x = ai_rand_rebornx();
    int y = 1;
    int xs = x;

    // Avoid guard to be born in holes or where gold lays.
    // TODO: Add gold check.
    while (!is_tile(game, x, y, MAP_TILE_EMPTY) || ai_hole(game, x, y)) {
        x = ai_rand_rebornx();
        if (x == xs) {
            // We have tried all positions this row, let's move to the next one.
            y++;
            if (y == MAP_HEIGHT) {
                die("guard cannot be born");
            }
        }
    }

    guard->x = x;
    guard->y = y;
    guard->hole = false;
    guard->holey = -1;
    guard->state = GSTATE_REBORN;
    guard->cura = guard_state_animation(guard, GSTATE_REBORN);

    // If guard dies still holding gold means that he could not drop it earlier.
    // Gold must be discarded in this case as a result runner have to pickup
    // one gold less.
    if (guard->gold != NULL) {
        game_discard_gold(game, guard->gold);
        guard->gold = NULL;
        guard->goldholds = 0;
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

    // Regular (running) guards move logic.
    int moves = move_policy[game->nguards][imoves];
    while (moves-- > 0) {
        if (++iguard >= game->nguards) {
            iguard = 0;
        }

        struct guard *g = game->guards[iguard];
        if (g->state == GSTATE_TRAP_LEFT
            || g->state == GSTATE_TRAP_RIGHT
            || g->state == GSTATE_REBORN) {
            continue;
        }

        enum dir d = ai_scan(game, g);
        ai_move_guard(game, g, d);
    }

    // Rebornd and trapped guards climbing out logic.
    for (int i = 0; i < game->nguards; i++) {
        struct guard *g = game->guards[i];

        if (g->state == GSTATE_TRAP_LEFT
            || g->state == GSTATE_TRAP_RIGHT) {
            // TODO: Make sure the runner can dig 3 holes, traps 3 guards
            //       and run over them.
            if (animation_tick(g->cura)) {
                g->state = GSTATE_CLIMB_OUT;
                g->cura = guard_state_animation(g, GSTATE_CLIMB_OUT);
            }
        } else if (g->state == GSTATE_REBORN) {
            if (animation_tick(g->cura)) {
                g->state = GSTATE_FALL_RIGHT;
                g->cura = guard_state_animation(g, GSTATE_FALL_RIGHT);
            }
        }

        // Pick up gold when step over it.
        if (g->gold == NULL && g->goldholds == 0) {
            struct gold *gld = gold_pickup(game, g->x, g->y, g->tx, g->ty);
            if (gld != NULL) {
                g->gold = gld;
                g->goldholds = ai_rand_goldholds();
            }
        }

        // If guard walled up in the wall it becomes dead and should be reborn.
        if (g->state != GSTATE_REBORN
            && is_tile(game, g->x, g->y, MAP_TILE_BRICK)) {
            ai_reborn(game, g);
        }
    }
}

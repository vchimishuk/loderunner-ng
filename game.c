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

#include <assert.h>
#include <SDL2/SDL.h>
#include "ai.h"
#include "animation.h"
#include "exit.h"
#include "game.h"
#include "gold.h"
#include "guard.h"
#include "keyhole.h"
#include "level.h"
#include "phys.h"
#include "runner.h"
#include "sound.h"
#include "texture.h"
#include "xmalloc.h"

static const int RUNNER_DX = 8;
static const int RUNNER_DY = 9;

static int min(int a, int b)
{
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

static bool skip_keyhole(int key)
{
    return key == SDLK_RETURN || key == SDLK_ESCAPE;
}

static struct map_tile *map_tile_init(enum map_tile_t t, enum animation_t a,
    int row, int col)
{
    struct map_tile *m = xmalloc(sizeof(struct map_tile));

    if (t != MAP_TILE_EMPTY) {
        m->basea = animation_init(a);
    } else {
        m->basea = NULL;
    }
    m->cura = m->basea;
    m->baset = t;
    m->curt = m->baset;
    m->x = col * TILE_MAP_WIDTH;
    m->y = row * TILE_MAP_HEIGHT;

    return m;
}

static void map_tile_destroy(struct map_tile *t)
{
    if (t->cura != t->basea && t->curt != MAP_TILE_EMPTY) {
        animation_destroy(t->cura);
    }
    if (t->baset != MAP_TILE_EMPTY) {
        animation_destroy(t->basea);
    }
    free(t);
}

static void map_tile_reset(struct map_tile *tile)
{
    tile->curt = tile->baset;
    if (tile->cura != NULL && tile->cura != tile->basea) {
        animation_destroy(tile->cura);
    }
    tile->cura = tile->basea;
    if (tile->cura != NULL) {
        animation_reset(tile->cura);
    }
}

static struct ground_tile *ground_tile_init(int col)
{
    struct ground_tile *tl = xmalloc(sizeof(struct ground_tile));
    tl->a = animation_init(ANIMATION_GROUND);
    tl->x = col * TILE_GROUND_WIDTH;
    tl->y = MAP_HEIGHT * TILE_MAP_HEIGHT;

    return tl;
}

static void ground_tile_destroy(struct ground_tile *t)
{
    animation_destroy(t->a);
    free(t);
}

static struct sprite **text_sprites_init(char *s)
{
    int n = strlen(s);
    int escs = 0;
    for (int i = 0; i < n; i++) {
        if (s[i] == '\\') {
            escs++;
        }
    }

    int nsprites = n - escs + 1;
    struct sprite **sprites = xmalloc(sizeof(struct sprite *) * nsprites);
    for (int i = 0, j = 0; i < n; i++, j++) {
        char ch = s[i];
        int img = 0;
        if (ch == '\\') {
            i++;
            ch = s[i];
            img = 1;
        }

        int idx;
        if (img && ch == MAP_TILE_GOLD) {
            idx = 40;
        } else if (img && ch == MAP_TILE_GUARD) {
            idx = 41;
        } else if (ch == ' ') {
            idx = 43;
        } else if (ch >= 'A' && ch <= 'Z') {
            idx = 10 + ch - 'A';
        } else if (ch >= '0' && ch <= '9') {
            idx = 0 + ch - '0';
        } else {
            die("invalid sprite");
        }
        int r = idx / 10;
        int c = idx % 10;

        struct sprite *sp = xmalloc(sizeof(struct sprite));
        sp->texture = texture_get(TEXTURE_TEXT);
        sp->x = c * TILE_TEXT_WIDTH;
        sp->y = r * TILE_TEXT_HEIGHT;
        sp->w = TILE_TEXT_WIDTH;
        sp->h = TILE_TEXT_HEIGHT;
        sprites[j] = sp;
    }
    sprites[nsprites - 1] = NULL;

    return sprites;
}

static void text_sprites_destroy(struct sprite **s)
{
    struct sprite **p = s;
    while (*p != NULL) {
        free(*p);
        p++;
    }
    free(s);
}

static bool empty_tile(struct game *game, int x, int y)
{
    return is_tile(game, x, y, MAP_TILE_EMPTY)
        || is_tile(game, x, y, MAP_TILE_FALSE);
}

static void lives_inc(struct game *game)
{
    game->lives = min(game->lives + 1, 100);
    if (game->info_lives != NULL) {
        text_sprites_destroy(game->info_lives);
        game->info_lives = NULL;
    }
}

static void runner_tick(struct game *game, int key)
{
    struct runner *runner = game->runner;
    enum runner_state state = runner->state;
    bool move = false;
    int x = runner->x;
    int y = runner->y;
    int tx = runner->tx;
    int ty = runner->ty;

    if (state == RSTATE_DIG_LEFT || state == RSTATE_DIG_RIGHT) {
        animation_tick(runner->holelefta);
        animation_tick(runner->holerighta);

        bool replay = animation_tick(runner->cura);
        int gx = state == RSTATE_DIG_LEFT ? runner->x - 1 : runner->x + 1;
        int gy = runner->y;
        struct guard *g = game_guard_get(game, gx, gy);

        int hx = state == RSTATE_DIG_LEFT ? runner->x - 1 : runner->x + 1;
        int hy = runner->y + 1;

        // Digging animation reached its end, so it is time to get back
        // to the state runner was before digging.
        if (replay) {
            assert(game->map[hy][hx]->cura == NULL);
            game->map[hy][hx]->cura = animation_init(ANIMATION_HOLE_FILL);
            state = state == RSTATE_DIG_LEFT ? RSTATE_LEFT : RSTATE_RIGHT;
        } else if (g != NULL) {
            // If runner moves over the hole when it is still in progress
            // we should rollback the digging process.
            if (g->ty > TILE_MAP_HEIGHT / 4) {
                game->map[hy][hx]->curt = game->map[hy][hx]->baset;
                assert(game->map[hy][hx]->cura == NULL);
                game->map[hy][hx]->cura = game->map[hy][hx]->basea;
                state = state == RSTATE_DIG_LEFT ? RSTATE_LEFT : RSTATE_RIGHT;
            }
        }
    } else if ((state == RSTATE_FALL_LEFT || state == RSTATE_FALL_RIGHT)
        || ((empty_tile(game, x, y + 1) || is_tile(game, x, y + 1, MAP_TILE_ROPE))
            && !is_tile(game, x, y, MAP_TILE_ROPE)
            && !is_tile(game, x, y, MAP_TILE_LADDER)
            && game_guard_get(game, x, y + 1) == NULL)) {
        // Continue falling process or start falling if runner is stending on
        // empty tile.

        // Start falling.
        if (state == RSTATE_LEFT || state == RSTATE_CLIMB_LEFT) {
            state = RSTATE_FALL_LEFT;
            sound_play(SOUND_FALL);
        } else if (state == RSTATE_RIGHT || state == RSTATE_CLIMB_RIGHT
            || state == RSTATE_UPDOWN) {

            state = RSTATE_FALL_RIGHT;
            sound_play(SOUND_FALL);
        }
        move = true;
        tx = 0;
        ty += RUNNER_DY;
        if (ty > TILE_MAP_HEIGHT / 2) {
            y += 1;
            ty -= TILE_MAP_HEIGHT;
        }

        // Grab the rope when falling. Grab it only if runner was falling
        // initially and not dropped the rope he was hanging before. Which
        // can be detected by ty value.
        if (is_tile(game, x, y, MAP_TILE_ROPE) && (ty > 0 && ty < RUNNER_DY)) {
            ty = 0;
            if (state == RSTATE_FALL_LEFT) {
                state = RSTATE_CLIMB_LEFT;
            } else {
                state = RSTATE_CLIMB_RIGHT;
            }
            sound_stop();
            sound_play(SOUND_DOWN);
        } else if (ty >= 0
            && ((!empty_tile(game, x, y + 1)
                    && !is_tile(game, x, y + 1, MAP_TILE_ROPE))
                || game_guard_get(game, x, y + 1) != NULL)) {
            // Stop, we have reached some solid ground.
            ty = 0;
            state = RSTATE_STOP;
            sound_stop();
            sound_play(SOUND_DOWN);
        }
    } else if (key != 0) {
        switch (key) {
        case SDLK_LEFT:
            tx -= RUNNER_DX;
            ty = 0;
            if (tx < -(TILE_MAP_WIDTH / 2)) {
                x -= 1;
                tx += TILE_MAP_WIDTH;
            }
            if (tx < 0 && !can_move(game, x - 1, y)) {
                move = false;
            } else {
                if (is_tile(game, x, y, MAP_TILE_ROPE)) {
                    state = RSTATE_CLIMB_LEFT;
                } else {
                    state = RSTATE_LEFT;
                }
                move = true;
            }
            break;
        case SDLK_RIGHT:
            tx += RUNNER_DX;
            ty = 0;
            if (tx > TILE_MAP_WIDTH / 2) {
                x += 1;
                tx -= TILE_MAP_WIDTH;
            }
            if (tx > 0 && !can_move(game, x + 1, y)) {
                move = false;
            } else {
                if (is_tile(game, x, y, MAP_TILE_ROPE)) {
                    state = RSTATE_CLIMB_RIGHT;
                } else {
                    state = RSTATE_RIGHT;
                }
                move = true;
            }
            break;
        case SDLK_UP:
            ty -= RUNNER_DY;
            tx = 0;
            if (ty < -(TILE_MAP_HEIGHT / 2)) {
                y -= 1;
                ty += TILE_MAP_HEIGHT;
            }
            bool onladder = is_tile(game, x, y, MAP_TILE_LADDER);
            if (ty < 0 && (!onladder || !can_move(game, x, y - 1))) {
                move = false;
            } else {
                state = RSTATE_UPDOWN;
                move = true;
            }
            break;
        case SDLK_DOWN:
            ty += RUNNER_DY;
            tx = 0;
            if (ty > TILE_MAP_HEIGHT / 2) {
                y += 1;
                ty -= TILE_MAP_HEIGHT;
            }

            if (state == RSTATE_UPDOWN && ty < 0 && empty_tile(game, x, y)) {
                // Runner starts falling down from the ladder.
                state = RSTATE_FALL_RIGHT;
                move = true;
            } else if (ty > 0 && !can_move(game, x, y + 1)
                && !is_tile(game, x, y + 1, MAP_TILE_FALSE)) {
                // Cannot move down any more -- ladder standing on the ground.
                move = false;
            } else {
                if (is_tile(game, x, y, MAP_TILE_ROPE)) {
                    // Hanging on the rope, drop it, start falling down
                    // or climb over ladder if any.
                    if (is_tile(game, x, y + 1, MAP_TILE_LADDER)) {
                        state = RSTATE_UPDOWN;
                    } else {
                        if (state == RSTATE_CLIMB_RIGHT) {
                            state = RSTATE_FALL_RIGHT;
                        } else {
                            state = RSTATE_FALL_LEFT;
                        }
                        sound_play(SOUND_FALL);
                    }
                } else {
                    // Keep moving down over the ladder.
                    state = RSTATE_UPDOWN;
                }
                move = true;
            }
            break;
        case SDLK_x:
            // Dig only bricks with empty gold-free space above.
            if (is_tile(game, x + 1, y + 1, MAP_TILE_BRICK)
                && is_tile(game, x + 1, y, MAP_TILE_EMPTY)
                && gold_get(game, x + 1, y) == NULL
                && game_guard_get(game, x + 1, y) == NULL) {

                struct map_tile *t = game->map[runner->y + 1][runner->x + 1];
                // Make sure we do not need to free animation.
                assert(t->cura == t->basea);
                t->cura = NULL;
                t->curt = MAP_TILE_EMPTY;
                state = RSTATE_DIG_RIGHT;
                animation_reset(runner->holerighta);
                sound_play(SOUND_DIG);
                tx = 0;
                move = true;
            } else {
                move = false;
            }
            break;
        case SDLK_z:
            // Dig only bricks with empty space above.
            if (is_tile(game, x - 1, y + 1, MAP_TILE_BRICK)
                && is_tile(game, x - 1, y, MAP_TILE_EMPTY)
                && gold_get(game, x - 1, y) == NULL
                && game_guard_get(game, x + 1, y) == NULL) {

                struct map_tile *t = game->map[runner->y + 1][runner->x - 1];
                // Make sure we do not need to free animation.
                assert(t->cura == t->basea);
                t->cura = NULL;
                t->curt = MAP_TILE_EMPTY;
                state = RSTATE_DIG_LEFT;
                animation_reset(runner->holelefta);
                sound_play(SOUND_DIG);
                tx = 0;
                move = true;
            } else {
                move = false;
            }
            break;
        }
    }
    if (move) {
        runner->x = x;
        runner->y = y;
        runner->tx = tx;
        runner->ty = ty;
        animation_tick(runner->cura);
    }
    if (runner->state != state) {
        // When stopped keep current animation.
        if (state != RSTATE_STOP) {
            runner->cura = runner_state_animation(runner, state);
            animation_reset(runner->cura);
        }
        runner->state = state;
    }
}

static void map_tick(struct game *game)
{
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            struct map_tile *t = game->map[i][j];
            if (t != NULL && t->cura != NULL) {
                bool replay = animation_tick(t->cura);
                if (replay) {
                    map_tile_reset(t);
                }
            }
        }
    }
}

static void open_hladder(struct game *g)
{
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            struct map_tile *t = g->map[i][j];
            if (t->baset == MAP_TILE_LADDER && t->curt == MAP_TILE_EMPTY) {
                map_tile_reset(t);
            }
        }
    }
}

/*
 * Check for a guard around the runner which we may collide with.
 */
static struct guard *guard_collision(struct game *g, struct runner *r)
{
    struct guard *gd;

    gd = game_guard_get(g, r->x, r->y);
    if (gd != NULL) {
        return gd;
    }

    // Look right.
    gd = game_guard_get(g, r->x + 1, r->y);
    if (gd != NULL) {
        if (gd->tx - r->tx < 0) {
            return gd;
        }
    }

    // Look down.
    gd = game_guard_get(g, r->x, r->y + 1);
    if (gd != NULL) {
        if (gd->ty - r->ty < 0) {
            return gd;
        }
    }

    // Look left.
    gd = game_guard_get(g, r->x - 1, r->y);
    if (gd != NULL) {
        if (r->tx - gd->tx < 0) {
            return gd;
        }
    }

    // Look up.
    gd = game_guard_get(g, r->x, r->y - 1);
    if (gd != NULL) {
        if (r->ty - gd->ty < 0) {
            return gd;
        }
    }

    return NULL;
}

/*
 * Check game state for collisions: runner or guard death.
 */
static void detect_collision(struct game *game)
{
    struct runner *r = game->runner;

    // Runner picks up gold.
    struct gold *g = gold_pickup(game, r->x, r->y, r->tx, r->ty);
    if (g != NULL) {
        r->ngold++;
        game_score(game, SCORE_GOLD);
        sound_play(SOUND_GOLD);
    }
    // All gold have been picked up. Show hidden ladders
    // and let the runner finish current game.
    if (!game->hladder && (r->ngold == game->ngold - game->lost_gold)) {
        open_hladder(game);
        game->hladder = true;
        sound_play(SOUND_HLADDER);
    }

    // Runner's death: walled up in a wall or hit by a guard.
    struct guard *guard = guard_collision(game, r);
    if (is_tile(game, r->x, r->y, MAP_TILE_BRICK) || guard != NULL) {
        game->state = GSTATE_END;
        game->keyhole = KH_MAX_RADIUS;
        game->level_score_iter = 0;
        sound_play(SOUND_DEAD);
    }

    // Runner has reached top of the screen.
    if (game->hladder && r->y == 0 && (abs(0 - r->ty) <= TILE_MAP_HEIGHT / 4)) {
        game->won = true;
        game->state = GSTATE_END;
        game->keyhole = KH_MAX_RADIUS;
        game->level_score_iter = 15;
        sound_play(SOUND_FINISH);
    }
}

static void runner_render(SDL_Renderer *renderer, struct runner *runner)
{
    animation_render(renderer, runner->cura,
            runner->x * TILE_MAP_WIDTH + runner->tx,
            runner->y * TILE_MAP_HEIGHT + runner->ty);

    if (runner->state == RSTATE_DIG_LEFT) {
        animation_render(renderer, runner->holelefta,
            (runner->x - 1) * TILE_MAP_WIDTH,
            (runner->y) * TILE_MAP_HEIGHT);
    }
    if (runner->state == RSTATE_DIG_RIGHT) {
        animation_render(renderer, runner->holerighta,
            (runner->x + 1) * TILE_MAP_WIDTH,
            (runner->y) * TILE_MAP_HEIGHT);
    }
}

static void guard_render(SDL_Renderer *renderer, struct guard *g)
{
    animation_render(renderer, g->cura,
            g->x * TILE_MAP_WIDTH + g->tx,
            g->y * TILE_MAP_HEIGHT + g->ty);
}

static void game_reset(struct game *game)
{
    game->lost_gold = 0;
    game->hladder = false;
    game->won = false;

    runner_reset(game->runner);

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            struct map_tile *t = game->map[i][j];
            map_tile_reset(t);
            // Special case for MAP_TILE_HLADDER.
            if (game->level->map[i][j] == MAP_TILE_HLADDER) {
                t->curt = MAP_TILE_EMPTY;
                t->cura = NULL;
            }
        }
    }

    for (int i = 0; i < game->nguards; i++) {
        guard_reset(game->guards[i]);
    }
    for (int i = 0; i < game->ngold; i++) {
        gold_reset(game->gold[i]);
    }
}

struct game *game_init(struct level *lvl)
{
    struct game *game = xmalloc(sizeof(struct game));
    game->state = GSTATE_START;
    game->keyhole = 0;
    game->level = lvl;
    game->lives = 5;
    game->info_score = NULL;
    game->info_lives = NULL;
    game->info_level = NULL;
    game->ngold = 0;
    game->lost_gold = 0;
    game->hladder = false;
    game->won = false;
    game->score = 0;
    game->runner = runner_init();
    game->guards = xmalloc(sizeof(struct guard *) * MAX_GUARDS);
    game->nguards = 0;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            switch (lvl->map[i][j]) {
            case MAP_TILE_BRICK:
                game->map[i][j] = map_tile_init(MAP_TILE_BRICK,
                    ANIMATION_BRICK, i, j);
                break;
            case MAP_TILE_EMPTY:
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY,
                    ANIMATION_NONE, i, j);
                break;
            case MAP_TILE_FALSE:
                game->map[i][j] = map_tile_init(MAP_TILE_FALSE,
                    ANIMATION_BRICK, i, j);
                break;
            case MAP_TILE_GOLD:
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY,
                    ANIMATION_NONE, i, j);
                if (game->ngold >= MAX_GOLD) {
                    die("gold limit exceeded");
                }
                game->gold[game->ngold++] = gold_init(j, i);
                break;
            case MAP_TILE_GUARD:
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY,
                    ANIMATION_NONE, i, j);

                struct guard *g = guard_init(j, i);
                game->nguards++;
                if (game->nguards > MAX_GUARDS) {
                    die("guard limit exceeded");
                }
                game->guards[game->nguards - 1] = g;
                break;
            case MAP_TILE_HLADDER:
                game->map[i][j] = map_tile_init(MAP_TILE_LADDER,
                    ANIMATION_LADDER, i, j);
                game->map[i][j]->curt = MAP_TILE_EMPTY;
                game->map[i][j]->cura = NULL;
                break;
            case MAP_TILE_LADDER:
                game->map[i][j] = map_tile_init(MAP_TILE_LADDER,
                    ANIMATION_LADDER, i, j);
                break;
            case MAP_TILE_ROPE:
                game->map[i][j] = map_tile_init(MAP_TILE_ROPE,
                    ANIMATION_ROPE, i, j);
                break;
            case MAP_TILE_RUNNER:
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY,
                    ANIMATION_NONE, i, j);
                game->runner->sx = j;
                game->runner->sy = i;
                runner_reset(game->runner);
                break;
            case MAP_TILE_SOLID:
                game->map[i][j] = map_tile_init(MAP_TILE_SOLID,
                    ANIMATION_SOLID, i, j);
                break;
            default:
                die("TODO");
            }
        }
    }

    for (int i = 0; i < MAP_WIDTH; i++) {
        game->ground[i] = ground_tile_init(i);
    }

    return game;
}

void game_destroy(struct game *game)
{
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map_tile_destroy(game->map[i][j]);
        }
    }

    for (int i = 0; i < MAP_WIDTH; i++) {
        ground_tile_destroy(game->ground[i]);
    }

    if (game->info_score != NULL) {
        text_sprites_destroy(game->info_score);
        game->info_score = NULL;
    }
    if (game->info_lives != NULL) {
        text_sprites_destroy(game->info_lives);
        game->info_lives = NULL;
    }
    if (game->info_level != NULL) {
        text_sprites_destroy(game->info_level);
        game->info_level = NULL;
    }

    runner_destroy(game->runner);

    for (int i = 0; i < game->nguards; i++) {
        guard_destroy(game->guards[i]);
    }
    free(game->guards);
    free(game);
}

void game_render(struct game *game, SDL_Renderer *renderer)
{
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            struct map_tile *t = game->map[i][j];
            if (t != NULL && t->cura != NULL) {
                animation_render(renderer, t->cura, t->x, t->y);
            }
        }
    }

    for (int i = 0; i < game->ngold; i++) {
        struct gold *g = game->gold[i];
        if (!g->lost && g->visible) {
            animation_render(renderer, g->animation,
                g->x * TILE_MAP_WIDTH, g->y * TILE_MAP_HEIGHT);
        }
    }

    runner_render(renderer, game->runner);

    for (int i = 0; i < game->nguards; i++) {
        guard_render(renderer, game->guards[i]);
    }

    for (int i = 0; i < MAP_WIDTH; i++) {
        struct ground_tile *t = game->ground[i];
        animation_render(renderer, t->a, t->x, t->y);
    }

    int col = 0;
    int infoy = MAP_HEIGHT * TILE_MAP_HEIGHT + TILE_GROUND_HEIGHT;
    if (game->info_score == NULL) {
        char buf[16];
        snprintf(buf, 16, "SCORE%07d", game->score);
        game->info_score = text_sprites_init(buf);

    }
    for (int i = 0; game->info_score[i] != NULL; i++, col++) {
        animation_render_sprite(renderer, game->info_score[i],
            col * TILE_TEXT_WIDTH, infoy);
    }
    if (game->info_lives == NULL) {
        char buf[16];
        snprintf(buf, 16, " MEN%03d", game->lives);
        game->info_lives = text_sprites_init(buf);
    }
    for (int i = 0; game->info_lives[i] != NULL; i++, col++) {
        animation_render_sprite(renderer, game->info_lives[i],
            col * TILE_TEXT_WIDTH, infoy);
    }
    if (game->info_level == NULL) {
        char buf[16];
        snprintf(buf, 16, " LEVEL%03d", game->level->num);
        game->info_level = text_sprites_init(buf);
    }
    for (int i = 0; game->info_level[i] != NULL; i++, col++) {
        animation_render_sprite(renderer, game->info_level[i],
            col * TILE_TEXT_WIDTH, infoy);
    }

    if (game->state == GSTATE_START || game->state == GSTATE_END) {
        keyhole_render(renderer, (int) game->keyhole);
    }
}

/*
 * Game tick function where all gameplay logic is happening. Called with
 * a frame rate speed.
 * Returns ture if game is finished. Game result is stored in `game->won`
 * flag.
 */
bool game_tick(struct game *game, int key)
{
    switch (game->state) {
    case GSTATE_END:
        if (game->level_score_iter > 0) {
            game_score(game, SCORE_FINISH_STEP);
            game->level_score_iter--;
            if (game->level_score_iter == 0) {
                lives_inc(game);
            }
        } else if (game->keyhole > 0) {
            if (skip_keyhole(key)) {
                game->keyhole = 0;
            } else {
                game->keyhole -= KH_SPEED;
            }
        } else if (game->won) {
            return true;
        } else {
            game->lives--;
            if (game->info_lives != NULL) {
                text_sprites_destroy(game->info_lives);
                game->info_lives = NULL;
            }
            if (game->lives > 0) {
                game_reset(game);
                game->state = GSTATE_START;
            } else {
                return true;
            }
        }
        break;
    case GSTATE_START:
        if (game->keyhole < KH_MAX_RADIUS) {
            if (skip_keyhole(key)) {
                game->keyhole = KH_MAX_RADIUS;
            } else {
                game->keyhole += KH_SPEED;
            }
        } else if (key != 0) {
            game->state = GSTATE_RUN;
        }
        break;
    case GSTATE_RUN:
        map_tick(game);
        runner_tick(game, key);
        ai_tick(game);
        detect_collision(game);
        break;
    }

    return false;
}

void game_score(struct game *game, int score)
{
    game->score += score;

    // Free score display textures to request its update.
    if (game->info_score != NULL) {
        text_sprites_destroy(game->info_score);
        game->info_score = NULL;
    }
}

// Return guard at x:y coordinate if there is any.
struct guard *game_guard_get(struct game *g, int x, int y)
{
    for (int i = 0; i < g->nguards; i++) {
        if (g->guards[i]->x == x && g->guards[i]->y == y) {
            return g->guards[i];
        }
    }

    return NULL;
}

#include <SDL2/SDL.h>
#include "animation.h"
#include "exit.h"
#include "gold.h"
#include "tile.h"
#include "game.h"
#include "keyhole.h"
#include "level.h"
#include "render.h"
#include "runner.h"
#include "texture.h"
#include "xmalloc.h"

#define RUNNER_DX 8
#define RUNNER_DY 9

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

static void map_tile_reset(struct map_tile *tile)
{
    tile->curt = tile->baset;
    tile->cura = tile->basea;
    if (tile->cura != NULL) {
        animation_reset(tile->cura);
    }
}

static struct ground_tile *ground_tile_init(int col)
{
    struct ground_tile *tl = xmalloc(sizeof(struct ground_tile));
    tl->sprite = animation_sprite_init(TEXTURE_GROUND, 0, 0,
        TILE_GROUND_WIDTH, TILE_GROUND_HEIGHT);
    tl->x = col * TILE_GROUND_WIDTH;
    tl->y = MAP_HEIGHT * TILE_MAP_HEIGHT;

    return tl;
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
            die("TODO:");
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

static bool is_tile(struct game *game, int x, int y, enum map_tile_t t)
{
    return game->map[y][x]->curt == t;
}

static struct gold *gold(struct game *g, int x, int y)
{
    for (int i = 0; i < g->ngold; i++) {
        struct gold *gl = g->gold[i];

        if (gl->visible && gl->x == x && gl->y == y) {
            return gl;
        }
    }

    return NULL;
}

static bool can_move(struct game *game, int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }

    return is_tile(game, x, y, MAP_TILE_EMPTY)
        || is_tile(game, x, y, MAP_TILE_LADDER)
        || is_tile(game, x, y, MAP_TILE_ROPE);
}

// TODO: Check correct usage.
static bool empty_tile(struct game *game, int x, int y)
{
    // TODO: Support false brick.
    return is_tile(game, x, y, MAP_TILE_EMPTY);
}

// TODO: Free digging hole animation.
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

        int replay = animation_tick(runner->cura);
        // Digging animation reached its end, so it is time to get back
        // to the state runner was before digging.
        if (replay) {
            struct animation *fill = animation_init(ANIMATION_HOLE_FILL);

            if (state == RSTATE_DIG_LEFT) {
                game->map[runner->y + 1][runner->x - 1]->cura = fill;
                state = RSTATE_LEFT;
            } else {
                game->map[runner->y + 1][runner->x + 1]->cura = fill;
                state = RSTATE_RIGHT;
            }
        }
    } else if ((state == RSTATE_FALL_LEFT || state == RSTATE_FALL_RIGHT)
        || (empty_tile(game, x, y + 1) && !is_tile(game, x, y, MAP_TILE_ROPE))) {
        // Continue falling process or start falling if runner is stending on
        // empty brick.

        if (state == RSTATE_FALL_LEFT || state == RSTATE_CLIMB_LEFT
            || state == RSTATE_LEFT) {
            state = RSTATE_FALL_LEFT;
        } else {
            state = RSTATE_FALL_RIGHT;
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
        } else if ((!empty_tile(game, x, y + 1)
                && !is_tile(game, x, y + 1, MAP_TILE_ROPE))
            && ty >= 0) {

            // Stop, we have reached some solid ground.
            ty = 0;
            state = RSTATE_STOP;
        }
    } else if (key != 0) {
        move = true;

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
            }
            break;
        case SDLK_DOWN:
            ty += RUNNER_DY;
            tx = 0;
            if (ty > TILE_MAP_HEIGHT / 2) {
                y += 1;
                ty -= TILE_MAP_HEIGHT;
            }
            if (ty > 0 && !can_move(game, x, y + 1)) {
                move = false;
            } else {
                if (is_tile(game, x, y, MAP_TILE_ROPE)
                    && !is_tile(game, x, y + 1, MAP_TILE_LADDER)) {

                    if (state == RSTATE_CLIMB_RIGHT) {
                        state = RSTATE_FALL_RIGHT;
                    } else {
                        state = RSTATE_FALL_LEFT;
                    }
                } else {
                    state = RSTATE_UPDOWN;
                }
            }
            break;
        case SDLK_x:
            // TODO: Do not dig if guard is too close.

            // Dig only bricks with empty gold-free space above.
            if (is_tile(game, x + 1, y + 1, MAP_TILE_BRICK)
                && is_tile(game, x + 1, y, MAP_TILE_EMPTY)
                && gold(game, x + 1, y) == NULL) {

                runner->tx = 0;
                game->map[runner->y + 1][runner->x + 1]->cura = NULL;
                game->map[runner->y + 1][runner->x + 1]->curt = MAP_TILE_EMPTY;
                state = RSTATE_DIG_RIGHT;
                animation_reset(runner->holerighta);
            } else {
                move = false;
            }
            break;
        case SDLK_z:
            // Dig only bricks with empty space above.
            if (is_tile(game, x - 1, y + 1, MAP_TILE_BRICK)
                && is_tile(game, x - 1, y, MAP_TILE_EMPTY)
                && gold(game, x - 1, y) == NULL) {

                runner->tx = 0;
                game->map[runner->y + 1][runner->x - 1]->cura = NULL;
                game->map[runner->y + 1][runner->x - 1]->curt = MAP_TILE_EMPTY;
                state = RSTATE_DIG_LEFT;
                animation_reset(runner->holelefta);
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
                    t->cura = t->basea;
                    t->curt = t->baset;
                }
            }
        }
    }
}

static void open_hladder(struct game *g)
{
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (g->lvl->map[i][j] == MAP_TILE_HLADDER) {
                // TODO: Free empty tile g->map[i][j].
                g->map[i][j] = map_tile_init(MAP_TILE_LADDER,
                    ANIMATION_LADDER, i, j);
            }
        }
    }
}

/*
 * Check game state for collisions: runner or guard death.
 * TODO: Gold picking up or lossing here?
 */
static void detect_collision(struct game *game)
{
    // TODO: Also check runner-guard collision here.
    struct runner *r = game->runner;

    // Runner picks up gold.
    struct gold *g = gold(game, r->x, r->y);
    if (g != NULL
        && abs(0 - r->tx) <= TILE_MAP_WIDTH / 4
        && abs(0 - r->ty) <= TILE_MAP_HEIGHT / 4) {

        g->visible = false;
        r->ngold++;

        // All gold have been picked up. Show hidden ladders
        // and let the runner finish current game.
        if (game->ngold == r->ngold) {
            open_hladder(game);
        }
    }

    // Runner is walled up in a wall.
    if (is_tile(game, r->x, r->y, MAP_TILE_BRICK)) {
        game->state = GSTATE_END;
        game->keyhole = KH_MAX_RADIUS;
    }

    // Runner has reached top of the screen.
    if (r->y == 0 && (abs(0 - r->ty) <= TILE_MAP_HEIGHT / 4)
        && game->ngold == r->ngold) {

        game->won = true;
        game->state = GSTATE_END;
    }
}

// TODO: Do we need to pass game or runner struct can be enough?
static void runner_render(SDL_Renderer *renderer, struct game *game)
{
    struct runner *runner = game->runner;

    render(renderer, *(runner->cura->cur),
            runner->x * TILE_MAP_WIDTH + runner->tx,
            runner->y * TILE_MAP_HEIGHT + runner->ty);

    if (runner->state == RSTATE_DIG_LEFT) {
        render(renderer, *(runner->holelefta->cur),
            (runner->x - 1) * TILE_MAP_WIDTH,
            (runner->y) * TILE_MAP_HEIGHT);
    }
    if (runner->state == RSTATE_DIG_RIGHT) {
        render(renderer, *(runner->holerighta->cur),
            (runner->x + 1) * TILE_MAP_WIDTH,
            (runner->y) * TILE_MAP_HEIGHT);
    }
}

static void game_reset(struct game *game)
{
    runner_reset(game->runner);
    // TODO: Decrement men.
    // TODO: Reset map: guards, gold, etc. Reset all tiles.
    // TODO: Reset statistics?

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map_tile_reset(game->map[i][j]);
        }
    }
}

struct game *game_init(SDL_Renderer *renderer, struct level *lvl)
{
    struct game *game = xmalloc(sizeof(struct game));
    game->state = GSTATE_START;
    game->keyhole = 0;
    game->lvl = lvl;
    game->lives = 5;
    game->info_score = NULL;
    game->info_lives = NULL;
    game->info_level = NULL;
    game->ngold = 0;
    game->won = false;
    game->runner = runner_init();

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            switch (lvl->map[i][j]) {
            case MAP_TILE_BRICK:
                game->map[i][j] = map_tile_init(MAP_TILE_BRICK,
                    ANIMATION_BRICK, i, j);
                break;
            case MAP_TILE_EMPTY:
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY, 0, i, j);
                break;
            case MAP_TILE_FALSE:
                // TODO:
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY, 0, i, j);
                break;
            case MAP_TILE_GOLD:
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY, 0, i, j);
                if (game->ngold >= MAX_GOLD) {
                    die("gold limit exceeded");
                }
                game->gold[game->ngold++] = gold_init(j, i);
                break;
            case MAP_TILE_GUARD:
                // TODO:
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY, 0, i, j);
                break;
            case MAP_TILE_HLADDER:
                // TODO:
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY, 0, i, j);
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
                game->map[i][j] = map_tile_init(MAP_TILE_EMPTY, 0, i, j);
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

// TODO: 1. Pick up gold when on the middle of the tile.
//       2. Do not dig under gold.
void game_render(struct game *game, SDL_Renderer *renderer)
{
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            struct map_tile *t = game->map[i][j];
            if (t != NULL && t->cura != NULL) {
                render(renderer, *(t->cura->cur), t->x, t->y);
            }
        }
    }

    for (int i = 0; i < game->ngold; i++) {
        struct gold *g = game->gold[i];
        if (g->visible) {
            render(renderer, *(g->animation->cur),
                g->x * TILE_MAP_WIDTH, g->y * TILE_MAP_HEIGHT);
        }
    }

    runner_render(renderer, game);

    for (int i = 0; i < MAP_WIDTH; i++) {
        struct ground_tile *t = game->ground[i];
        render(renderer, t->sprite, t->x, t->y);
    }

    int col = 0;
    int yinfo = MAP_HEIGHT * TILE_MAP_HEIGHT + TILE_GROUND_HEIGHT;
    if (game->info_score == NULL) {
        char buf[16];
        snprintf(buf, 16, "SCORE%07d", 100500);
        game->info_score = text_sprites_init(buf);

    }
    for (int i = 0; game->info_score[i] != NULL; i++, col++) {
        render(renderer, game->info_score[i], col * TILE_TEXT_WIDTH, yinfo);
    }
    if (game->info_lives == NULL) {
        char buf[16];
        snprintf(buf, 16, " MEN%03d", game->lives);
        game->info_lives = text_sprites_init(buf);
    }
    for (int i = 0; game->info_lives[i] != NULL; i++, col++) {
        render(renderer, game->info_lives[i], col * TILE_TEXT_WIDTH, yinfo);
    }
    if (game->info_level == NULL) {
        char buf[16];
        snprintf(buf, 16, " LEVEL%03d", game->lvl->num);
        game->info_level = text_sprites_init(buf);
    }
    for (int i = 0; game->info_level[i] != NULL; i++, col++) {
        render(renderer, game->info_level[i], col * TILE_TEXT_WIDTH, yinfo);
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
        if (game->keyhole > 0) {
            game->keyhole -= KH_SPEED;
        } else if (game->won) {
            return true;
        } else {
            game->lives--;
            game->info_lives = NULL; // TODO: Free memory / reset.
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
            game->keyhole += KH_SPEED;
        } else if (key != 0) {
            game->state = GSTATE_RUN;
        }
        break;
    case GSTATE_RUN:
        map_tick(game);
        runner_tick(game, key);
        detect_collision(game);
        break;
    }

    // TODO: Make game_render() static and call it here instead of main.c?

    return false;
}

void game_destroy(struct game *game)
{
    // TODO: Free map.
    // TODO: Free gold.
    free(game);
}

#include <SDL2/SDL.h>
#include "exit.h"
#include "game.h"
#include "level.h"
#include "render.h"
#include "runner.h"
#include "texture.h"
#include "xmalloc.h"

#define COL_INFO_SCORE 0
#define COL_INFO_LIFE 13
#define COL_INFO_LEVEL 20

#define RUNNER_DX 8

static struct tile *map_tile_init(enum texture t, int row, int col)
{
    struct tile *tl = xmalloc(sizeof(struct tile));
    tl->texture = texture_get(t);
    tl->x = col * TILE_MAP_WIDTH;
    tl->y = row * TILE_MAP_HEIGHT;
    tl->tx = 0;
    tl->ty = 0;
    tl->tw = TILE_MAP_WIDTH;
    tl->th = TILE_MAP_HEIGHT;

    return tl;
}

static struct tile *ground_tile_init(int col)
{
    struct tile *tl = xmalloc(sizeof(struct tile));
    tl->texture = texture_get(TEXTURE_GROUND);
    tl->x = col * TILE_GROUND_WIDTH;
    tl->y = MAP_HEIGHT * TILE_MAP_HEIGHT;
    tl->tx = 0;
    tl->ty = 0;
    tl->tw = TILE_GROUND_WIDTH;
    tl->th = TILE_GROUND_HEIGHT;

    return tl;
}

static struct tile **text_tile_init(char *s, int col)
{
    int n = strlen(s);
    int escs = 0;
    for (int i = 0; i < n; i++) {
        if (s[i] == '\\') {
            escs++;
        }
    }

    int ntiles = n - escs + 1;
    struct tile **tiles = xmalloc(sizeof(struct tile *) * ntiles);
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
        } else if (ch >= 'A' && ch <= 'Z') {
            idx = 10 + ch - 'A';
        } else if (ch >= '0' && ch <= '9') {
            idx = 0 + ch - '0';
        } else {
            die("TODO:");
        }
        int r = idx / 10;
        int c = idx % 10;

        struct tile *tl = xmalloc(sizeof(struct tile));
        tl->texture = texture_get(TEXTURE_TEXT);
        tl->x = (col + j) * TILE_TEXT_WIDTH;
        tl->y = MAP_HEIGHT * TILE_MAP_HEIGHT + TILE_GROUND_HEIGHT;
        tl->tx = c * TILE_TEXT_WIDTH;
        tl->ty = r * TILE_TEXT_HEIGHT;
        tl->tw = TILE_TEXT_WIDTH;
        tl->th = TILE_TEXT_HEIGHT;
        tiles[j] = tl;
    }
    tiles[ntiles - 1] = NULL;

    return tiles;
}

static void move_runner(struct game *game, int key)
{
    struct runner *runner = game->runner;

    // TODO: Moving to the next tile.
    switch (key) {
    case SDLK_LEFT:
        runner->x += RUNNER_DX;
        break;
    case SDLK_RIGHT:
        runner->x -= RUNNER_DX;
        break;
    }
}

struct game *game_init(SDL_Renderer *renderer, struct level *lvl)
{
    struct game *game = xmalloc(sizeof(struct game));
    game->lvl = lvl;
    game->runner = runner_init();

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            switch (lvl->map[i][j]) {
            case MAP_TILE_BRICK:
                game->map[i][j] = map_tile_init(TEXTURE_BRICK, i, j);
                break;
            case MAP_TILE_EMPTY:
                game->map[i][j] = NULL;
                break;
            case MAP_TILE_FALSE:
                game->map[i][j] = NULL;
                break;
            case MAP_TILE_GOLD:
                game->map[i][j] = NULL;
                break;
            case MAP_TILE_GUARD:
                game->map[i][j] = NULL;
                break;
            case MAP_TILE_HLADDER:
                game->map[i][j] = NULL;
                break;
            case MAP_TILE_LADDER:
                game->map[i][j] = map_tile_init(TEXTURE_LADDER, i, j);
                break;
            case MAP_TILE_ROPE:
                game->map[i][j] = map_tile_init(TEXTURE_ROPE, i, j);
                break;
            case MAP_TILE_RUNNER:
                game->map[i][j] = NULL;
                game->runner->x = j;
                game->runner->y = i;
                break;
            case MAP_TILE_SOLID:
                game->map[i][j] = map_tile_init(TEXTURE_SOLID, i, j);
                break;
            default:
                die("TODO");
            }
        }
    }

    for (int i = 0; i < MAP_WIDTH; i++) {
        game->ground[i] = ground_tile_init(i);
    }

    char buf[16];
    snprintf(buf, 16, "SCORE%07d", 5);
    game->info_score = text_tile_init(buf, COL_INFO_SCORE);
    snprintf(buf, 16, "MEN%03d", 5);
    game->info_life = text_tile_init(buf, COL_INFO_LIFE);
    snprintf(buf, 16, "LEVEL%03d", 5);
    game->info_level = text_tile_init(buf, COL_INFO_LEVEL);

    return game;
}

void game_render(struct game *game, SDL_Renderer *renderer)
{
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            struct tile *t = game->map[i][j];
            if (t != NULL) {
                render(renderer, t);
            }
        }
    }

    struct runner *r = game->runner;
    r->tile->x = r->x * TILE_MAP_WIDTH + r->tx;
    r->tile->y = r->y * TILE_MAP_HEIGHT + r->ty;
    render(renderer, r->tile);

    for (int i = 0; i < MAP_WIDTH; i++) {
        render(renderer, game->ground[i]);
    }
    for (int i = 0; game->info_score[i] != NULL; i++) {
        render(renderer, game->info_score[i]);
    }
    for (int i = 0; game->info_life[i] != NULL; i++) {
        render(renderer, game->info_life[i]);
    }
    for (int i = 0; game->info_level[i] != NULL; i++) {
        render(renderer, game->info_level[i]);
    }
}

void game_tick(struct game *game, int key)
{
    move_runner(game, key);
}

void game_destroy(struct game *game)
{
    // TODO: Free map.
    free(game);
}

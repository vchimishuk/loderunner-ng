#ifndef GAME_H_
#define GAME_H_

#include "level.h"
// #include "tile.h"

/*
 * Game map single tile representation.
 */
struct map_tile {
    // Base animation for this tile, animation displayed by default. E.g. brick.
    struct animation *basea;
    // Currently displayed animation on the current map tile. Usually same as
    // base animation but can be different in some periods of the game. For
    // example, when runner digs a hole current animatio is set to hole animation
    // instead of brick animation.
    struct animation *cura;
    // Tile's base type.
    enum map_tile_t baset;
    // Tile's current type. Can be different from base type at some periods
    // during game. For example, when hole is dug tile's type is changed from
    // brick to empty for period of time when hole is active. Returned back to
    // base_t after that.
    enum map_tile_t curt;
    // X screen coordinate to draw current animation at. It is not always the
    // same as actual map tile coordinate as some animations (like hole
    // animation) takes space of two map tiles.
    int x;
    // Y screen coordinate to draw current animation at.
    int y;
};

struct ground_tile {
    struct sprite *sprite;
    int x;
    int y;
};

enum game_state {
    /* // Runner run out of lives, GAME OVER screen is shown. */
    /* GSTATE_END, */
    // Game is running, user can control runner.
    GSTATE_RUN,
    // Game round is going to start, keyhole is shown or waiting for initial
    // user's command.
    GSTATE_START,
};

/*
 * Game represents single level playing process. It stores gaming state of the
 * current level, like active textures, their location, location of runner,
 * guards, gold and so on.
 */
struct game {
    enum game_state state;
    float keyhole;
    struct level *lvl;
    struct map_tile *map[MAP_HEIGHT][MAP_WIDTH];
    struct ground_tile *ground[MAP_WIDTH];
    struct sprite **info_score;
    struct sprite **info_life;
    struct sprite **info_level;
    struct runner *runner;
};

struct game *game_init(SDL_Renderer *renderer, struct level *lvl);
void game_tick(struct game *game, int key);
void game_render(struct game *game, SDL_Renderer *renderer);
void game_destroy();

#endif /* GAME_H_ */

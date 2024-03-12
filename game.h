#ifndef GAME_H_
#define GAME_H_

#include "level.h"
#include "tile.h"

/*
 * Game represents single level playing process. It stores gaming state of the
 * current level, like active textures, their location, location of runner,
 * guards, gold and so on.
 */
struct game {
    struct level *lvl;
    struct tile *map[MAP_HEIGHT][MAP_WIDTH];
    struct tile *ground[MAP_WIDTH];
    struct tile **info_score;
    struct tile **info_life;
    struct tile **info_level;
    struct runner *runner;
};

struct game *game_init(SDL_Renderer *renderer, struct level *lvl);
void game_tick(struct game *game, int key);
void game_render(struct game *game, SDL_Renderer *renderer);
void game_destroy();

#endif /* GAME_H_ */

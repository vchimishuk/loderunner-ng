#ifndef LEVEL_H_
#define LEVEL_H_

#define MAP_WIDTH 28
#define MAP_HEIGHT 16

enum map_tile_t {
    MAP_TILE_BRICK = '#',
    MAP_TILE_EMPTY = ' ',
    MAP_TILE_FALSE = 'X',
    MAP_TILE_GOLD = '$',
    MAP_TILE_GUARD = '0',
    MAP_TILE_HLADDER = 'S',
    MAP_TILE_LADDER = 'H',
    MAP_TILE_ROPE = '-',
    MAP_TILE_RUNNER = '&',
    MAP_TILE_SOLID = '@',
};

struct level {
    int num;
    enum map_tile_t map[MAP_HEIGHT][MAP_WIDTH];
};

struct level *load_level(int n);

#endif /* LEVEL_H_ */

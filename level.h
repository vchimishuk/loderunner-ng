#ifndef LEVEL_H_
#define LEVEL_H_

#define MAP_WIDTH 28
#define MAP_HEIGHT 16

#define MAP_TILE_BRICK '#'
#define MAP_TILE_EMPTY ' '
#define MAP_TILE_FALSE 'X'
#define MAP_TILE_GOLD '$'
#define MAP_TILE_GUARD '0'
#define MAP_TILE_HLADDER 'S'
#define MAP_TILE_LADDER 'H'
#define MAP_TILE_ROPE '-'
#define MAP_TILE_RUNNER '&'
#define MAP_TILE_SOLID '@'

struct level {
    char map[MAP_HEIGHT][MAP_WIDTH];
};

struct level *load_level(int n);

#endif /* LEVEL_H_ */

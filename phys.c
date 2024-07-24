#include "phys.h"

// Check if tile at x:y coordinates has requested type.
bool is_tile(struct game *game, int x, int y, enum map_tile_t t)
{
    return game->map[y][x]->curt == t;
}

// Returns true if runner or guard can move to tile with x:y coordinates.
bool can_move(struct game *game, int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }

    return is_tile(game, x, y, MAP_TILE_EMPTY)
        || is_tile(game, x, y, MAP_TILE_LADDER)
        || is_tile(game, x, y, MAP_TILE_ROPE);
}

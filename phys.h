#ifndef PHYS_H_
#define PHYS_H_

#include <stdbool.h>
#include "game.h"

#define MOVE_DX 8
#define MOVE_DY 9

bool is_tile(struct game *game, int x, int y, enum map_tile_t t);
bool can_move(struct game *game, int x, int y);

#endif /* PHYS_H_ */

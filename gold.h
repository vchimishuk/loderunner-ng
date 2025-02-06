#ifndef GOLD_H_
#define GOLD_H_

#include "animation.h"

struct gold {
    int sx;
    int sy;
    int x;
    int y;
    // Flag indicating whether the gold should be rendered or not.
    // When runner or guard picks the gold up we simply hide it
    // and display it back when it is dropped.
    bool visible;
    // When guard holding gold dies he drops gold on the ground. However,
    // it can be a situation when there is not place to drop gold to
    // (for example guard is walled deep into a wall). In this case gold
    // he holds become completely lost and cannot be picked up bu the
    // runner any more.
    bool lost;
    struct animation *animation;
};

struct gold *gold_init(int x, int y);
void gold_destroy(struct gold *gold);
void gold_reset(struct gold *gold);
struct gold *gold_get(struct game *g, int x, int y);
struct gold *gold_pickup(struct game *g, int x, int y, int tx, int ty);
void gold_drop(struct gold *g, int x, int y);
void gold_lose(struct gold *g);

#endif /* GOLD_H_ */

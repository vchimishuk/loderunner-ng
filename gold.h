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
    struct animation *animation;
};

struct gold *gold_init(int x, int y);
void gold_reset(struct gold *gold);
void gold_destroy(struct gold *gold);

#endif /* GOLD_H_ */

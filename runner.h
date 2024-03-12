#ifndef RUNNER_H_
#define RUNNER_H_

struct runner {
    // X (0..MAP_WIDTH) position on the map.
    int x;
    // Y (0..MAP_HEIGHT) position on the map.
    int y;
    // X offset in the current map tile. When runner moves to the second
    // half of the current tile it is transfered to the next tile. To keep
    // runner on the same spot on screen tx becomes negative.
    int tx;
    // Same as tx but for vertical movement: falls and ladders.
    int ty;
    // "Move left" animation tiles. NULL-teminated list.
    struct tile **ltiles;
    // "Move right" animation tiles. NULL-teminated list.
    struct tile **rtiles;
    // Currently rendering tile. Points to one of the elements
    // in ltiles or rtiles.
    struct tile *tile;

    // TODO: State like falling.
};

struct runner *runner_init();

#endif /* RUNNER_H_ */

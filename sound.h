#ifndef SOUND_H_
#define SOUND_H_

enum sound {
    SOUND_DEAD,
    SOUND_DIG,
    SOUND_DOWN,
    SOUND_FALL,
    SOUND_GOLD,
    SOUND_HLADDER,
    SOUND_REBORN,
    SOUND_TRAP,
    SOUND_X_SIZE,
};

void sound_init();
void sound_destroy();
void sound_play(enum sound e);
void sound_stop(void);

#endif /* SOUND_H_ */

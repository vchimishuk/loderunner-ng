#include "SDL2/SDL_mixer.h"
#include "exit.h"
#include "sound.h"

#define SOUND_DIR "./sounds/"

Mix_Music *sounds[SOUND_X_SIZE];

static char *files[] = {
    SOUND_DIR "dead.ogg",    // SOUND_DEAD
    SOUND_DIR "dig.ogg",     // SOUND_DIG
    SOUND_DIR "down.ogg",    // SOUND_DOWN
    SOUND_DIR "fall.ogg",    // SOUND_FALL
    SOUND_DIR "finish.ogg",  // SOUND_FINISH
    SOUND_DIR "gold.ogg",    // SOUND_GOLD
    SOUND_DIR "hladder.ogg", // SOUND_HLADDER
    SOUND_DIR "reborn.ogg",  // SOUND_REBORN
    SOUND_DIR "trap.ogg",    // SOUND_TRAP
};

static Mix_Music *load(enum sound snd)
{
    char *f = files[snd];
    Mix_Music *m = Mix_LoadMUS(f);
    if (m == NULL) {
        die("failed to load sound resource: %s: %s", f, Mix_GetError());
    }

    return m;
}

void sound_init(void)
{
    sounds[SOUND_DEAD] = load(SOUND_DEAD);
    sounds[SOUND_DIG] = load(SOUND_DIG);
    sounds[SOUND_DOWN] = load(SOUND_DOWN);
    sounds[SOUND_FALL] = load(SOUND_FALL);
    sounds[SOUND_FINISH] = load(SOUND_FINISH);
    sounds[SOUND_GOLD] = load(SOUND_GOLD);
    sounds[SOUND_HLADDER] = load(SOUND_HLADDER);
    sounds[SOUND_REBORN] = load(SOUND_REBORN);
    sounds[SOUND_TRAP] = load(SOUND_TRAP);
}

void sound_destroy(void)
{
    Mix_FreeMusic(sounds[SOUND_DEAD]);
    Mix_FreeMusic(sounds[SOUND_DIG]);
    Mix_FreeMusic(sounds[SOUND_DOWN]);
    Mix_FreeMusic(sounds[SOUND_FALL]);
    Mix_FreeMusic(sounds[SOUND_FINISH]);
    Mix_FreeMusic(sounds[SOUND_GOLD]);
    Mix_FreeMusic(sounds[SOUND_HLADDER]);
    Mix_FreeMusic(sounds[SOUND_REBORN]);
    Mix_FreeMusic(sounds[SOUND_TRAP]);
    Mix_Quit();
}

void sound_pause(void)
{
    static int paused = 0;

    paused = !paused;
    Mix_PauseAudio(paused);
}

void sound_play(enum sound snd)
{
    Mix_PlayMusic(sounds[snd], 0);
}

void sound_stop(void)
{
    Mix_HaltMusic();
}

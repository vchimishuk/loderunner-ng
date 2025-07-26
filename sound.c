// Copyright 2024-2025 Viacheslav Chimishuk <vchimishuk@yandex.ru>
//
// This file is part of loderunner-ng.
//
// loderunner-ng is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// loderunner-ng is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with loderunner-ng. If not, see <http://www.gnu.org/licenses/>.

#include "SDL2/SDL_mixer.h"
#include "config.h"
#include "exit.h"
#include "sound.h"

Mix_Music *sounds[SOUND_X_SIZE];

static char *files[] = {
    SOUNDS_DIR "/dead.ogg",    // SOUND_DEAD
    SOUNDS_DIR "/dig.ogg",     // SOUND_DIG
    SOUNDS_DIR "/down.ogg",    // SOUND_DOWN
    SOUNDS_DIR "/fall.ogg",    // SOUND_FALL
    SOUNDS_DIR "/finish.ogg",  // SOUND_FINISH
    SOUNDS_DIR "/gold.ogg",    // SOUND_GOLD
    SOUNDS_DIR "/hladder.ogg", // SOUND_HLADDER
    SOUNDS_DIR "/reborn.ogg",  // SOUND_REBORN
    SOUNDS_DIR "/trap.ogg",    // SOUND_TRAP
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

void sound_volume(int v)
{
    Mix_VolumeMusic(v / 100.0 * MIX_MAX_VOLUME);
}

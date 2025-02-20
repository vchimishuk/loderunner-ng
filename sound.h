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

#ifndef SOUND_H_
#define SOUND_H_

enum sound {
    SOUND_DEAD,
    SOUND_DIG,
    SOUND_DOWN,
    SOUND_FALL,
    SOUND_FINISH,
    SOUND_GOLD,
    SOUND_HLADDER,
    SOUND_REBORN,
    SOUND_TRAP,
    SOUND_X_SIZE,
};

void sound_init(void);
void sound_destroy(void);
void sound_pause(void);
void sound_play(enum sound e);
void sound_stop(void);
void sound_volume(int v);

#endif /* SOUND_H_ */

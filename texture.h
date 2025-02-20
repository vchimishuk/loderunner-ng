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

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL.h>

enum texture {
    TEXTURE_BRICK,
    TEXTURE_EMPTY,
    TEXTURE_GOLD,
    TEXTURE_GROUND,
    TEXTURE_GUARD,
    TEXTURE_HLADDER,
    TEXTURE_HOLE,
    TEXTURE_LADDER,
    TEXTURE_PAUSED,
    TEXTURE_ROPE,
    TEXTURE_RUNNER,
    TEXTURE_SOLID,
    TEXTURE_TEXT,
    TEXTURE_TRAP,
    // Keep it last.
    TEXTURE_SIZE,
};

SDL_Texture *texture_load(SDL_Renderer *renderer, char *file);
void texture_init(SDL_Renderer *renderer);
void texture_destroy(void);
SDL_Texture *texture_get(enum texture t);

#endif /* TEXTURE_H_ */

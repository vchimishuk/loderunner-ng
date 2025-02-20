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

#ifndef KEYHOLE_H_
#define KEYHOLE_H_

#include <SDL2/SDL.h>

#define KH_PIXEL 8
// Original game takes about 5 seconds to show the scene.
#define KH_SPEED 0.7
#define KH_SCREEN_WIDTH (40 * 28 / KH_PIXEL)
#define KH_SCREEN_HEIGHT (44 * 16 / KH_PIXEL)
#define KH_MAX_RADIUS (sqrtf(KH_SCREEN_WIDTH * KH_SCREEN_WIDTH  \
            + KH_SCREEN_HEIGHT * KH_SCREEN_HEIGHT) / 2)

void keyhole_render(SDL_Renderer *renderer, int r);

#endif /* KEYHOLE_H_ */

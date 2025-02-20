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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "exit.h"
#include "level.h"
#include "path.h"
#include "xmalloc.h"

#define LEVELS_DIR "./levels"

static const char *TILE_CHARS = "# $0SH-&@X";

/*
 * Load level from file.
 * It is caller's responsibility to free returned object.
 */
struct level *level_init(int n)
{
    char buf[4];
    snprintf(buf, 4, "%03d", n % 1000);
    char *fname = path_join(LEVELS_DIR, buf);
    int f = open(fname, O_RDONLY);
    if (f == -1) {
        die("failed to load level %s: %s", fname, strerror(errno));
    }

    struct level *lvl = xmalloc(sizeof(struct level));
    lvl->num = n;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        char c;
        ssize_t n;
        int nl = 0;

        for (int j = 0; j < MAP_WIDTH; j++) {
            n = read(f, &c, 1);
            if (n == -1) {
                die("failed to read %s: %s", fname, strerror(errno));
            } else if (n == 0 || c == '\n') {
                for (; j < MAP_WIDTH; j++) {
                    lvl->map[i][j] = MAP_TILE_EMPTY;
                }
                nl = 1;
                goto eol;
            }

            char *t = strchr(TILE_CHARS, c);
            if (t == NULL) {
                die("invalid level file format: unsupported tile %c", c);
            }
            lvl->map[i][j] = c;
        }
    eol:
        if (!nl) {
            n = read(f, &c, 1);
            if (n == -1) {
                die("failed to read %s: %s", fname, strerror(errno));
            }
            if (n == 0 && i != MAP_HEIGHT - 1) {
                die("invalid level file format %s", fname);
            }
        }
    }

    close(f);
    free(fname);

    return lvl;
}

void level_destroy(struct level *l)
{
    free(l);
}

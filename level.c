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

/* /\* */
/*  * Convert character representation as in level-file to struct tile (int). */
/*  * Returns -1 if character is not valid. */
/*  *\/ */
/* static int parse_tile(char c) */
/* { */
/*     char *p = strchr(TILE_CHARS, c); */
/*     if (p == NULL) { */
/*         return -1; */
/*     } */

/*     return p - TILE_CHARS; */
/* } */

/*
 * Load level from file.
 * It is caller's responsibility to free returned object.
 */
struct level *load_level(int n)
{
    char buf[4];
    snprintf(buf, 4, "%03d", n % 1000);
    char *fname = path_join(LEVELS_DIR, buf);
    int f = open(fname, O_RDONLY);
    if (f == -1) {
        die("failed to load level %s: %s", fname, strerror(errno));
    }

    struct level *lvl = xmalloc(sizeof(struct level));

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

// TODO: Just for debugging. Remove it.
void print_level(struct level *lvl)
{
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            printf("%c", lvl->map[i][j]);
        }
        printf("\n");
    }
}

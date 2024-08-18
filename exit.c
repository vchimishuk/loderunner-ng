#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_error.h>
#include "exit.h"

void die(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    exit(EXIT_FAILURE);
}

void die_sdl(char *func)
{
    const char *sdl = SDL_GetError();
    char *buf = NULL;
    asprintf(&buf, "%s: %s", func, sdl);
    die(buf);
}

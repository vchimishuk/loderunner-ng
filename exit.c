#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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

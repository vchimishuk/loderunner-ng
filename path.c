#include <stdio.h>
#include <string.h>
#include "xmalloc.h"

/*
 * Join two FS paths. It is caller's responsibility to free memory
 * allocated by the returned string.
 */
char *path_join(char *a, char *b)
{
    size_t n = strlen(a) + 1 + strlen(b) + 1;
    char *s = xmalloc(n);

    snprintf(s, n, "%s/%s", a, b);

    return s;
}

#include <stdlib.h>
#include "exit.h"

void *xmalloc(size_t size)
{
    void *p = malloc(size);
    if (p == NULL) {
        die("malloc failed");
    }

    return p;
}

void xfree(void *p)
{
    free(p);
}

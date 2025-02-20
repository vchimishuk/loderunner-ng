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

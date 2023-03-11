#include "lib.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <err.h>
#include <termios.h>

void
die(char *msg)
{
    perror(msg);
    exit(1);
}

char *
strip(char *start, char *end, const char ch)
{
    size_t length;
    char *result, *new_start, *new_end;

    new_start = start;
    new_end = end;

    while (*new_start == ch && new_start < end) ++new_start;
    while (*new_end == ch && new_end > start) --new_end;

    length = str_length(new_start, new_end);
    result = calloc(length + 1, sizeof(char));

    if (!result) {
        perror("Failed call to calloc");
        free(result);
        return 0;
    }

    memmove(result, new_start, length * sizeof(char));
    result[length] = '\0';

    return result;
}

char *
str_alloc(const char *s)
{
    char *new = calloc(strlen(s) + 1, sizeof(char));
    if (!new) {
        die("Failed call to calloc");
    }

    strcpy(new, s);
    return new;
}

int
empty(char *s)
{
    return (int)*s ? 0 : 1;
}

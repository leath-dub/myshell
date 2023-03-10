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
    memmove(result, new_start, length);
    result[length] = '\0';

    return result;
}

char *
join(char **list, size_t length, char glue)
{
    char *joined;
    char *joined_end;
    size_t joined_length;
    size_t item_length;
    size_t new_length;

    joined = NULL;
    joined_length = 0;
    for (size_t i = 0; i < length; i++) {
        item_length = strlen(list[i]);
        new_length = (joined_length + item_length) + glue ? 1 : 0;
        joined = realloc(joined, new_length * sizeof(char));

        joined[joined_length] = glue;
        joined_end = joined + joined_length;
        *joined_end = glue;
        joined_end += glue ? 1 : 0;
        memmove(joined_end, list[i], item_length);
    }

    return joined;
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

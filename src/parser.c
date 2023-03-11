#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "parser.h"
#include "lib.h"

#define MAX_ARG 64

char *
parsearg(char *start, size_t length)
{
    char *tmp;
    char ch;
    int flags = 0;

    for (size_t i = 0; i < length; i += 1) {
        ch = start[i];
        if (ch == '\'') flags ^= STR_SINGLE_QUOTE_OPEN;
        if (ch == '\"') flags ^= STR_DOUBLE_QUOTE_OPEN;
        if (!flags && ch == IFS) {
            tmp = &start[i];
            while (*tmp == IFS) { *tmp = 0; tmp++; }
            return tmp;
        }
    }
    return &start[length - 1];
}

char **
parseargs(char *start, char *end, size_t *_length, size_t *_capacity)
{
    int invalid;
    int argument_count;
    size_t length;
    size_t capacity;
    char **argv;

    if (start == end) {
        return NULL;
    }

    if (empty(start)) { /* do nothing if its empty string */
        return NULL;
    }

    /* We need to allocate
     * char ** (arg vector to return) (args * sizeof (char))
     * char * (slice) (start - end)
     */

    char *copy = strip(start, end, ' ');

    length = 0;
    capacity = 16;
    argv = calloc(capacity, sizeof(char *));
    if (argv == NULL) {
        perror("Failed call to calloc");
        return NULL;
    }

    char *s = copy;
    char *copy_end = copy + strlen(copy);
    char *ret = s;
    argument_count = 0;
    loop {
        ret = parsearg(s, copy_end - ret + 1);
        vec_push_back(char *, argv, capacity, length, s);
        s = ret;
        if (ret == copy_end) break;
        argument_count += 1;
        if (argument_count >= MAX_ARG) {
            warn("reached maximum argument limit: %d\n", MAX_ARG);
            break;
        }
    }
    vec_push_back(char *, argv, capacity, length, 0); // null terminate


    /* make sure no NULL ptrs */
    if (_length) *_length = length - 1;
    if (_capacity) *_capacity = capacity;
    return argv;
}

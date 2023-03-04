#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "redirect.h"
#include "parser.h"
#include "lib.h"

/* Token map is used to map patterns to functions
 * token    token_l    proccess */
static const tok_map_t tokens[] = {
    {">", 1, NULL}, // TODO actuall link them to functions
    {"<", 1, NULL},
    {">>", 2, NULL},
    {"&", 1, NULL},
    0,
};

void
parser_parse_line(char *raw_input, int length)
{
    int pos;

    /* Iterate through the line, viewing each point as a substring from
     * i to the end. This substring is then checked against the special
     * tokens
     */
    for (unsigned int i = 0; i < length; i += 1) {
        pos = tok_match(&raw_input[i], length - i, tokens);
        if (pos != tok_failed_match) {
            i += tokens[pos].token_l - 1; // ensures things like ">>" dont match twice
            printf("match %s\n", tokens[pos].token);
        }
    }
}

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
    /*
    char *copy = calloc(str_length(start, end) + 1, sizeof(char));
    memmove(copy, start, str_length(start, end));
    */

    size_t length = 0;
    size_t capacity = 10;
    char **argv = calloc(capacity, sizeof(char *));

    char *s = copy;
    char *copy_end = copy + (end - start + 1);
    char *ret = s;
    loop {
        ret = parsearg(s, copy_end - ret + 1);
        vec_push_back(char *, argv, capacity, length, s);
        s = ret;
        if (ret == copy_end) break;
    }
    vec_push_back(char *, argv, capacity, length, 0); // null terminate


    /* make sure no NULL ptrs */
    if (_length) *_length = length;
    if (_capacity) *_capacity = capacity;
    return argv;
}

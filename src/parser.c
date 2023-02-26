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

char **
parseargs(char *start, char *end, size_t *_length, size_t *_capacity)
{
    if (start == end) {
        return NULL;
    }

    /* We need to allocate
     * char ** (arg vector to return) (args * sizeof (char))
     * char * (slice) (start - end)
     */

    char *copy = calloc(str_length(start, end), 1);
    memmove(copy, start, str_length(start, end));

    size_t length = 0;
    size_t capacity = 1;
    char **argv = calloc(capacity, sizeof(char *));

    /* now we can use strtok_r to tokenize on IFS */
    /* char *strtok_r(char *restrict str, const char *restrict delim,
                      char **restrict saveptr); */
    char *tok;
    tok = strtok(copy, (char[]){IFS, 0});
    while (tok) {
        vec_push_back(char *, argv, capacity, length, tok);
        tok = strtok(NULL, (char[]){IFS, 0});
        if (tok == NULL) {
            break;
        }
    }

    /* make sure no NULL ptrs */
    if (_length) *_length = length;
    if (_capacity) *_capacity = capacity;
    return argv;
}

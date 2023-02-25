#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "redirect.h"
#include "parser.h"

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
parseargs(char *start, char *end)
{
    if (start == end) {
        return NULL;
    }

    /* We need to allocate
     * char ** (arg vector to return) (args * sizeof (char))
     * char * (slice) (start - end)
     */

    char *copy = calloc(end - start, 1);
    char *saveptr;

    memmove(copy, start, end - start);

    /* now we can use strtok_r to tokenize on IFS */
    /* char *strtok_r(char *restrict str, const char *restrict delim,
                      char **restrict saveptr); */
    /* TODO: get strtok working here */
    printf("%s\n", copy);
    char *tok;
    tok = strtok(copy, " ");
    while (tok) {
        printf("%s\n", tok);
        tok = strtok(NULL, " ");
        if (tok == NULL) {
            break;
        }
    }

    return NULL;
}

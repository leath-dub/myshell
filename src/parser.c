#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "parser.h"
#include "lib.h"

#define MAX_ARG 64

/**
 * Parses single argument (similar to strtok)
 *
 * @param string start - string to parse argument from
 * @param size_t length - length of string
 * @returns string - reference address after last tab or space matched
 */
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
        if (!flags && (ch == ' ' || ch == '\t')) {
            tmp = &start[i];
            while (*tmp == ' ' || *tmp == '\t') {
                *tmp = '\0';
                tmp++;
            }
            return tmp;
        }
    }
    return &start[length - 1];
}

/**
 * Parses all arguments in slice into argument vector
 *
 * @param string start - start of string slice to parse
 * @param string end - end of string slice to parse
 * @param size_t *_length - pointer to writable memory address, where the
 *                          number of arguments will be written to
 * @param size_t *_capacity - pointer to writable memory address, where the
 *                            capacity of the argument array will be written
 * @returns string[] - array of arguments
 */
char **
parseargs(char *start, char *end, size_t *_length, size_t *_capacity)
{
    int argument_count;
    size_t length;
    size_t capacity;
    char **argv;
    char *argument;
    char *possible_argument;
    char *copy_end;

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

    char *copy = strip(start, end, " \t");

    length = 0;
    capacity = 16;
    argv = calloc(capacity, sizeof(char *));
    if (argv == NULL) {
        perror("Failed call to calloc");
        return NULL;
    }

    argument = copy;
    copy_end = copy + strlen(copy);
    possible_argument = argument;
    argument_count = 0;
    loop {
        possible_argument = parsearg(argument, copy_end - possible_argument + 1);
        vec_push_back(char *, argv, capacity, length, argument); /* add argument to arg vectory */
        argument = possible_argument;
        if (possible_argument == copy_end) break;
        argument_count += 1;

        /* We set a max argument as we don't want certain input to
         * force a infinate allocation loop */
        if (argument_count >= MAX_ARG) {
            warn("reached maximum argument limit: %d\n", MAX_ARG);
            break;
        }
    }
    vec_push_back(char *, argv, capacity, length, 0); /* null terminate */


    if (_length != NULL) *_length = length - 1;
    if (_capacity != NULL) *_capacity = capacity;
    return argv;
}

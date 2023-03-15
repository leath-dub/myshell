#include "lib.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <err.h>
#include <termios.h>

/* Any uncategorized functions that are shared between files
 * go here
 */

/**
 * Do not exit gracefully, die !.
 *
 * @param string msg - message to explain your mortality
 */
void
die(char *msg)
{
    perror(msg);
    exit(1);
}

/**
 * Removes any occurence of characters from start and end
 * in string slice. Like the python strip
 *
 * @param string start - start of string slice
 * @param string end - end of string slice
 * @param string chars - characters to remove from ends
 * @returns string - new string with characters removed
 */
char *
strip(char *start, char *end, const char *chars)
{
    size_t length;
    char *result, *new_start, *new_end;

    new_start = start;
    new_end = end;

    while (strchr(chars, *new_start) && new_start < end) ++new_start;
    while (strchr(chars, *new_end) && new_end > start) --new_end;

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

/**
 * Strcpy but guarded
 *
 * @param string s - string to copy
 * @returns string - the new string
 */
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

/**
 * Checks if string is empty
 *
 * @param string s - string to check
 * @returns bool - is the string empty
 */
int
empty(char *s)
{
    return (int)*s ? 0 : 1;
}

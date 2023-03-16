/*
Cathal O'Grady, 21442084 - Operating Systems Project 1
ca216/myshell

I understand that the University regards breaches of academic integrity and
plagiarism as grave and serious. I have read and understood the DCU Academic
Integrity and Plagiarism Policy. I accept the penalties that may be imposed
should I engage in practice or practices that breach this policy. I have
identified and included the source of all facts, ideas, opinions and
viewpoints of others in the assignment references. Direct quotations,
paraphrasing, discussion of ideas from books, journal articles, internet
sources, module text, or any other source whatsoever are acknowledged and the
sources cited are identified in the assignment references. I declare that
this material, which I now submit for assessment, is entirely my own work and
has not been taken from the work of others save and to the extent that such
work has been cited and acknowledged within the text of my work. I have used
the DCU library referencing guidelines (available at
https://www4.dcu.ie/library/classes_and_tutorials/citingreferencing.shtml
and/or the appropriate referencing system recommended in the assignment
guidelines and/or programme documentation. By signing this form or by
submitting material online I confirm that this assignment, or any part of it,
has not been previously submitted by me or any other person for assessment on
this or any other course of study By signing this form or by submitting
material for assessment online I confirm that I have read and understood the
DCU Academic Integrity and Plagiarism Policy (available at
http://www.dcu.ie/registry/examinations/index.shtml).
*/

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

    /* @ref https://man7.org/linux/man-pages/man3/strcpy.3.html */
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

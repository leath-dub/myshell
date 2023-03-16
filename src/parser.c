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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

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
            /* @ref (use of warn and err) https://git.codemadness.org/xscreenshot/file/xscreenshot.c.html */
            warn("reached maximum argument limit: %d\n", MAX_ARG);
            break;
        }
    }
    vec_push_back(char *, argv, capacity, length, 0); /* null terminate */


    if (_length != NULL) *_length = length - 1;
    if (_capacity != NULL) *_capacity = capacity;
    return argv;
}

/**
 * [[helper]] Return reference to correct fd based on the type of command
 *
 * @param struct c - command to reference
 * @param int type - type of command
 * @returns int * - reference to file descriptor
 */
static int *
getcmdfd(struct cmd *c, int type)
{
    switch(type) {
        case REDRO:
            return &c->fdout;
        case REDRI:
            return &c->fdin;
    }
    return 0;
}

/**
 * Abstract function for parseing io redirection, could be "stdout write",
 * "stdout append" or "stdin read" - respective child functions parsewrite,
 * parseappend and parseread. It opens file with flags.
 *
 * @param struct c - command to write open file descriptors to
 * @param string start - start pointer of command
 * @param string match - pointer to end of matched token (">", "<" or ">>")
 * @param size_t match_length - length of matched token
 * @param string end - pointer to end of command (could be pointing at next
 *                     matched token or end of commandline)
 * @param int flags - open flags : flags passed to open function, e.g. O_RDWR
 * @param int type - type of io redirection (REDRO or REDRI)
 * @returns int - return code
 */
int
parseio(struct cmd *c, char *start, char *match, size_t match_length, char *end, int flags, int type)
{
    int *fd;
    char *rhs;

    fd = getcmdfd(c, type);
    if (!fd) return -1;

    if (bin_isset_flag(c->flags, type)) {
        close(*fd);
    }
    bin_set_flag(c->flags, type);

    /* file on the right hand side of match */
    if (match == end) {
        fprintf(stderr, "invalid right operand\n");
        return 0;
    }
    rhs = strip(match + 1, end, " \t"); // strip leading/trailing space

    /* @ref https://man7.org/linux/man-pages/man2/openat.2.html */
    *fd = open(rhs, flags, 0666);
    if (*fd < 0) {
        perror(rhs);

        /* clean up */
        bin_unset_flag(c->flags, type);
        *fd = 0; // when runcmd sees 0, it sais "inherit parent fd's"
        free(rhs);
        return -1;
    }
    free(rhs);

    /* echo 'hello' >> world  # this is how you get lhs operand
     * ^           ^ ^
     * start       | match
     *             match - match_length
     */
    parseexec(c, start, match - match_length);
    return 0;
}

/**
 * Utilizes parseio with write flags for open
 *
 * ... see parseio params up to `end`
 */
int
parsewrite(struct cmd *c, char *start, char *match, size_t match_length, char *end)
{
    return parseio(c, start, match, match_length, end, O_CREAT | O_WRONLY | O_TRUNC, REDRO);
}

/**
 * Utilizes parseio with read flags for open
 *
 * ... see parseio params up to `end`
 */
int
parseread(struct cmd *c, char *start, char *match, size_t match_length, char *end)
{
    return parseio(c, start, match, match_length, end, O_RDONLY, REDRI);
}

/**
 * Utilizes parseio with append flags for open
 *
 * ... see parseio params up to `end`
 */
int
parseappend(struct cmd *c, char *start, char *match, size_t match_length, char *end)
{
    return parseio(c, start, match, match_length, end, O_WRONLY | O_CREAT | O_APPEND, REDRO);
}

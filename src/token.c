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

#include "token.h"

/* This file implements a token matcher. The token matcher works by
 * matching characters against columns of each items token field in a queryset
 * of type tok_map_t. This is used for commandline parsing in parser.c, it
 * allows you to map a token to a function.
 */


/**
 * Part of the token matcher, will be called multiple times the caller will
 * increment the column each time
 *
 * @param char q - query character ( what to match on )
 * @param struct toks - token set to match on
 * @param int column - column index of token
 * @returns int - return code enum {tok_failed_match, token_more_match}
 *                else the position (index) in the token map of the match
 */
int
tok_match_iter(char q, const tok_map_t *toks, int column)
{
    int pos = 0;
    int not_overflow;
    int column_matches;
    int at_the_end;

    /* *(int *)tok just checks that tok is not null (compiler suppressed :]) */
    for (const tok_map_t *tok = toks; *(int *)tok; tok += 1) {
        /* guard against index range and check whether character
         * at given column matches
         */
        not_overflow = tok->token_l > column;
        column_matches = tok->token[column] == q;
        if (not_overflow && column_matches) {
            at_the_end = tok->token_l - 1 == column;
            if (at_the_end) {
                return pos;
            }
            return tok_more_match; /* tell caller there is more to match */
        }
        pos += 1;
    }

    return tok_failed_match;
}

/**
 * Utilizes tok_match_iter to match against strings
 *
 * @param string q - string pattern query ( what to match on )
 * @param int len - length of the string q
 * @param struct toks - token set to match on
 * @returns int - return code or position
 */
int
tok_match(char *q, int len, const tok_map_t *toks)
{
    int any_match;
    int rc;
    int i;

    any_match = tok_failed_match;
    rc = tok_more_match;

    for (i = 0; i < len && rc != tok_failed_match; i += 1) {
        rc = tok_match_iter(q[i], toks, i);
        if (rc != tok_failed_match) {
            any_match = rc;
        }
    }
    if (i != 0) { /* some match was found at any point */
        return any_match;
    }
    return rc;
}

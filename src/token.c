/* This file implements a token matcher. The token matcher works by
 * matching characters against columns of each items token field in a queryset
 * of type tok_map_t. This is used for commandline parsing in parser.c, it
 * allows you to map a token to a function.
 */

#include <stdint.h>

#include "token.h"

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

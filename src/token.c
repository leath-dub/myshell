#include <stdint.h>

#include "token.h"

int
tok_match_iter(char q, const tok_map_t *toks, int column)
{
    int pos = 0;
    for (const tok_map_t *tok = toks; *(int *)tok; tok += 1) {
        if (tok->token_l > column && tok->token[column] == q) {
            if (tok->token_l - 1 == column) {
                return pos;
            }
            return tok_more_match;
        }
        pos += 1;
    }
    return tok_failed_match;
}

int
tok_match(char *q, int len, const tok_map_t *toks)
{
    int any_match = tok_failed_match;
    int rc = tok_more_match;
    int i;
    for (i = 0; i < len && rc != tok_failed_match; i += 1) {
        rc = tok_match_iter(q[i], toks, i);
        if (rc != tok_failed_match) {
            any_match = rc;
        }
    }
    if (i != 0) { /* some match was found */
        return any_match;
    }
    return rc;
}

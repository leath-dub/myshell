#include <unistd.h>
#include "command.h"

typedef struct {
    char *token;
    size_t token_l;
    tok_fn process;
} tok_map_t;

enum tok_match_t {
    tok_failed_match = -1,
    tok_more_match = -2,
};

int tok_match_iter(char q, const tok_map_t *toks, int column);
int tok_match(char *q, int len, const tok_map_t *toks);

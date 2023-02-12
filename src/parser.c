#include "token.h"
#include "redirect.h"
#include "parser.h"

/* Token map is used to map patterns to functions
 * token    token_l    proccess */
static const tok_map_t tokens[] = {
    {">", 1, redirect_stdout},
    {"<", 1, redirect_stdin},
    {">>", 2, redirect_stdin_append},
    {"&", 1, job_background_lhs},
    0,
};

static job_t *lhs = 0;
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

/*
int
main(int argc, char **argv)
{
    char *line = "echo 'hello'&<inputfile>>outputfile";
    int length = 37;
    parser_parse_line(line, length);
    return 0;
}
*/

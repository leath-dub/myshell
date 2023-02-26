#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "parser.h"
#include "command.h"
#include "token.h"
#include "lib.h"


/* TODO use open_memestream to store commandline buffer */

/* Token map is used to map patterns to functions
 * token    token_l    proccess */
static const tok_map_t tokens[] = {
    {">", 1, parsewrite},
    {"<", 1, parseread},
    {">>", 2, parseappend},
    {0, 0, 0},
};

int
parseexec(struct cmd *c, char *start, char *end)
{
    /* if no command, create one with lhs */
    if (!bin_isset_flag(c->flags, EXEC)) {
        c->argv = parseargs(start, end, (size_t *)&c->argc, NULL);
        if (c->argv != 0) bin_set_flag(c->flags, EXEC);
    }
    return 0;
}

/* return ref to fd field based on given type */
int *
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
    rhs = strip(match + 1, end, ' '); // strip leading/trailing space

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
     *              |^ match
     *              ^ match - match_length
     * the extra 1 is because of 0 indexing
     */
    parseexec(c, start, match - 1 - match_length);
    return 0;
}

/* TODO: add the lhs parsing to make a command to exec */
int
parsewrite(struct cmd *c, char *start, char *match, size_t match_length, char *end)
{
    return parseio(c, start, match, match_length, end, O_CREAT | O_WRONLY | O_TRUNC, REDRO);
}

int
parseread(struct cmd *c, char *start, char *match, size_t match_length, char *end)
{
    return parseio(c, start, match, match_length, end, O_RDONLY, REDRI);
}

int
parseappend(struct cmd *c, char *start, char *match, size_t match_length, char *end)
{
    return parseio(c, start, match, match_length, end, O_WRONLY | O_CREAT | O_APPEND, REDRO);
}

int
parseback(struct cmd *c, char *start, char *match, size_t match_length, char *end)
{
    bin_set_flag(c->flags, BACK);

    /* change the character to a space, so it is ignored */
    *match = ' ';

    return 0;
}

int
checkback(char *line, size_t length)
{
    char ch;
    int back;
    int flags = 0;

    back = 0;
    for (size_t i = 0; i < length; i += 1) {
        ch = line[i];
        if (ch == '\'') bin_toggle_flag(flags, STR_SINGLE_QUOTE_OPEN);
        if (ch == '\"') bin_toggle_flag(flags, STR_DOUBLE_QUOTE_OPEN);
        if (!flags && ch == '&') { // no open strings
            line[i] = ' ';
            back = 1;
        }
    }
    return back;
}


struct cmd *
parsecmd(char *parseme, size_t length)
{
    struct cmd *c = calloc(1, sizeof(struct cmd));
    if (!c) {
        die("Failed call to calloc");
    }

    /* remove any & that are un escaped or not in a string */
    if (checkback(parseme, length)) {
        bin_set_flag(c->flags, BACK);
    }

    /* below is an algorithm that utilizes 2 queues, it basically keeps a
     * buffer of matches so that you can parse text between tokens or the
     * end of the line
     *
     * we also Iterate through the line, viewing each point as a substring from
     * i to the end. This substring is then checked against the special
     * tokens
     */ // TODO all parsing should be done in parser.c
    int matchp;
    int matchq[2] = {-1, -1};          // stores match positions
    const tok_map_t *tokq[2] = {0, 0}; // stores match token values
    for (size_t i = 0; i < length; i += 1) {
        matchp = tok_match(&parseme[i], length - i, tokens);
        if (matchp != tok_failed_match) {
            i += tokens[matchp].token_l - 1; // ensures things like ">>" dont match twice

            tokq[1] = tokq[0];         // shift q and put current match
            tokq[0] = &tokens[matchp]; // in front, same for matchq below.
            matchq[1] = matchq[0];
            matchq[0] = i;

            if (matchq[1] > 0) {
                /* cast void * as tok fn */
                ((tok_fn)tokq[1]->process)(c, parseme, parseme + matchq[1],
                                           tokq[0]->token_l, parseme + matchq[0]
                                             - tokq[0]->token_l);
                /* tok_match returns the end position of the match so we need
                 * to minus its length to get the start
                 */
            }
        }
    }
    if (matchq[0] > 0) {
        ((tok_fn)tokq[0]->process)(c, parseme, parseme + matchq[0], tokq[0]->token_l, parseme + length - 1);
    }

    // may not have exec yet, if no match was found e.g
    parseexec(c, parseme, parseme + length - 1);

    return c;
}

int
runcmd(struct cmd *c)
{
    pid_t pid;
    int status;

    if (!bin_isset_flag(c->flags, EXEC)) return 0; // nothing to do
    if (!c->argv) return -1;

    pid = fork();
    if (pid == 0) { // child
        if (bin_isset_flag(c->flags, REDRI)) dup2(c->fdin, STDIN_FILENO);
        if (bin_isset_flag(c->flags, REDRO)) dup2(c->fdout, STDOUT_FILENO);
        execvp(c->argv[0], c->argv);
        perror("execvp");
    } else {
        if (!bin_isset_flag(c->flags, BACK)) { // run in background
            waitpid(pid, &status, 0);
        } // else don't wait, run in background
    }

    return 0;
}

void
printcmd(struct cmd *c)
{
    /* print the flags */
    printf("Command @%p\n", (void *)c);
    printf("| flags: ");
    printf("0x%x ", c->flags);
    if (bin_isset_flag(c->flags, BACK)) {
        printf("BACK ");
    }
    if (bin_isset_flag(c->flags, REDRI)) {
        printf("REDRI ");
    }
    if (bin_isset_flag(c->flags, REDRO)) {
        printf("REDRO ");
    }
    if (bin_isset_flag(c->flags, EXEC)) {
        printf("EXEC");
    }
    putchar('\n');

    printf("| fdout: ");
    if (bin_isset_flag(c->flags, REDRO) && c->fdout) {
        printf("%d", c->fdout);
    } else {
        printf("(nil)");
    }
    putchar('\n');

    printf("| fdin: ");
    if (bin_isset_flag(c->flags, REDRI) && c->fdin) {
        printf("%d", c->fdin);
    } else {
        printf("(nil)");
    }
    putchar('\n');

    printf("| argv: ");
    if (bin_isset_flag(c->flags, EXEC) && c->argv) {
        vec_print(char *, c->argv, c->argc, "%s");
    } else {
        printf("(nil)\n");
    }
}

int
cleancmd(struct cmd *c)
{
    int flags;

    flags = c->flags;

    /* close parents open file descriptors */
    if (bin_isset_flag(flags, REDRI)) { 
        close(c->fdin);
    }
    if (bin_isset_flag(flags, REDRO)) {
        close(c->fdout);
    }

    if (bin_isset_flag(flags, EXEC)) {
        free(*c->argv);
        free(c->argv);
    }
    free(c);

    return 0;
}

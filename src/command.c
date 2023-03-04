#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>
#include <stdlib.h>

#include "parser.h"
#include "command.h"
#include "token.h"
#include "lib.h"
#include "prompt.h"


extern char **environ;

/* from main.c */
extern char *path_to_shell;

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

static void
push_to_position_queue(int item, int *queue) {
    queue[1] = queue[0];
    queue[0] = item;
}

static void
push_to_token_queue(const tok_map_t *item, const tok_map_t **queue)
{
    queue[1] = queue[0];
    queue[0] = item;
}

/* function that processess from previous match to current match */
static void
process_previous_match(struct cmd *cmd, char *parseme, const tok_map_t **token_queue, int *match_position_queue)
{
    tok_fn process_match;
    char *start, *current_match, *next_match;
    size_t current_match_length;

    /* cast void * as tok fn */
    process_match = (tok_fn)token_queue[1]->process;
    start = parseme;
    current_match = start + match_position_queue[1];
    current_match_length = token_queue[0]->token_l;
    next_match = start + match_position_queue[0] - current_match_length;
    process_match(cmd, start, current_match, current_match_length,
                  next_match);
    /* tok_match returns the end position of the match so we need
     * to minus its length to get the start
     */
}

/* function to process any match remaining to the end of the string */
static void
process_any_match_left(struct cmd *cmd, char *parseme, size_t parseme_length, const tok_map_t **token_queue, int *match_position_queue)
{
    tok_fn process_match;
    char *start, *current_match, *end;
    size_t current_match_length;

    process_match = (tok_fn)token_queue[0]->process;
    start = parseme;
    current_match = parseme + match_position_queue[0];
    current_match_length = token_queue[0]->token_l;
    end = parseme + parseme_length - 1;
    process_match(cmd, start, current_match, current_match_length, end);
}

struct cmd *
parsecmd(char *parseme, size_t length)
{
    int match_position;
    int queue_is_full;
    int queue_has_item;
    int match_position_queue[2];
    int string_flags;
    const tok_map_t *token_queue[2];

    struct cmd *cmd = calloc(1, sizeof(struct cmd));
    if (!cmd) {
        die("Failed call to calloc");
    }

    /* remove any & that are un escaped or not in a string */
    if (checkback(parseme, length)) {
        bin_set_flag(cmd->flags, BACK);
    }

    /* below is an algorithm that utilizes 2 queues, it basically keeps a
     * buffer of matches so that you can parse text between tokens or the
     * end of the line
     *
     * we also Iterate through the line, viewing each point as a substring from
     * i to the end. This substring is then checked against the special
     * tokens
     */ // TODO all parsing should be done in parser.c
        // TODO ignore things in strings

    memset(match_position_queue, -1, 2 * sizeof(int));
    memset(token_queue, 0, 2 * sizeof(tok_map_t *));

    string_flags = 0;
    for (size_t i = 0; i < length; i += 1) {
        if (parseme[i] == '\'') bin_toggle_flag(string_flags, STR_SINGLE_QUOTE_OPEN);
        if (parseme[i] == '\"') bin_toggle_flag(string_flags, STR_DOUBLE_QUOTE_OPEN);
        if (string_flags) { // open strings
            continue;
        }

        match_position = tok_match(&parseme[i], length - i, tokens);
        if (match_position == tok_failed_match) {
            continue;
        }

        i += tokens[match_position].token_l - 1; // ensures things like ">>" dont match twice

        push_to_token_queue(&tokens[match_position], token_queue);
        push_to_position_queue(i, match_position_queue);

        queue_is_full = match_position_queue[1] != -1;
        if (!queue_is_full) {
            continue;
        }

        process_previous_match(cmd, parseme, token_queue, match_position_queue);
    }
    queue_has_item = match_position_queue[0] > 0;
    if (queue_has_item) {
        process_any_match_left(cmd, parseme, length, token_queue, match_position_queue);
    }

    // may not have exec yet, if no match was found e.g
    parseexec(cmd, parseme, parseme + length - 1);

    return cmd;
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

        /* set parent=<path to shell executable> */
        setenv("parent", path_to_shell, 0);

        execvp(c->argv[0], c->argv);
        perror(c->argv[0]);
        exit(1); // child exits
    } else {
        c->pid = pid; // set child pid
        if (!bin_isset_flag(c->flags, BACK)) { // run in background
            waitpid(pid, &status, 0);
        } // else don't wait, run in background
    }

    return 0;
}

#define print_if_true(cond, ...) if (cond) printf(__VA_ARGS__);

void
printcmd(struct cmd *c)
{
    /* print the flags */
    printf("Command @%p\n", (void *)c);
    printf("| pid: %d", c->pid);
    printf("| flags: ");
    printf("0x%x ", c->flags);

    print_if_true(bin_isset_flag(c->flags, BACK), "BACK ");
    print_if_true(bin_isset_flag(c->flags, REDRI), "REDRI ");
    print_if_true(bin_isset_flag(c->flags, REDRO), "REDRO ");
    print_if_true(bin_isset_flag(c->flags, EXEC), "EXEC");
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
    int cmd_is_null;

    cmd_is_null = c == NULL;
    if (cmd_is_null) {
        return 1;
    }

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

int
getcmd(int mode, FILE *stream, char *buf, size_t bufsz, size_t *bytes_read)
{
    size_t length;

    /* clean the buffer */
    memset(buf, 0, bufsz);

    if (mode == CMDINTER) {
        stream = stdin;
        draw_prompt();
        fflush(stdout);
    }

    if (mode == CMDBATCH && stream == NULL) {
        fprintf(stderr, "invalid stream\n");
        return 1;
    }

    if (fgets(buf, bufsz, stream) != buf) {
        return 1;
    }

    length = strlen(buf);
    if (buf[length - 1] == '\n') {
        buf[length - 1] = 0;
        length = length - 1;
    }
    *bytes_read = length;

    return 0;
}

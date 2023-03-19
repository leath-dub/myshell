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

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

/* because we are using ansi C, setenv is platform dependent
 * change this to your platform
 */
int setenv(const char *name, const char *value, int overwrite);

#include "parser.h"
#include "command.h"
#include "token.h"
#include "lib.h"
#include "prompt.h"
#include "builtin.h"


extern char **environ;

#include <linux/limits.h>
/* from main.c */
extern char path_to_shell[PATH_MAX];

/* Token map is used to map patterns to functions
 * token    token_l    proccess */
static const tok_map_t tokens[] = {
    {">", 1, parsewrite},
    {"<", 1, parseread},
    {">>", 2, parseappend},
    {0, 0, 0},
};

/**
 * Uses parseargs to "argify" the command into something you can
 * execute with e.g. exec
 *
 * @param struct c - command structure to write parsed command to
 * @param string start - start pointer of command (part of slice)
 * @param string end - end pointer of command (part of slice)
 * @returns int - return code
 */
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


/**
 * [[helper]] Shifts 2 item queue and inserts item at first position
 *
 * @param int item - item to add to queue
 * @param int[] queue - the 2 item queue
 */
static void
push_to_position_queue(int item, int *queue) {
    queue[1] = queue[0];
    queue[0] = item;
}

/**
 * [[helper]] Shifts 2 item tok_map_t queue and inserts item at first
 * position
 *
 * @param struct item - item to add to queue
 * @param struct[] queue - the 2 item queue
 */
static void
push_to_token_queue(const tok_map_t *item, const tok_map_t **queue)
{
    queue[1] = queue[0];
    queue[0] = item;
}

/**
 * Processess from previous match to current match
 *
 * @param struct cmd - command to pass to proccess_match
 * @param string parseme - string to be processed/parsed
 * @param struct[] token_queue - 2 item queue for storing matches
 * @param int[] match_position_queue - 2 item queue for storing index of matches
 */
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

/**
 * Process any match remaining up to the end of the string
 *
 * @param struct cmd - command to pass to proccess_match
 * @param string parseme - string to be processed/parsed
 * @param size_t parseme_length - length of string to be processed, parseme
 * @param struct[] token_queue - 2 item queue for storing matches
 * @param int[] match_position_queue - 2 item queue for storing index of matches
 */
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

/**
 * Searches command line for '&' that is not in string and sets background
 * flag
 *
 * @param string line - command
 * @param size_t length - length of command
 * @returns int - return code
 */
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

/**
 * Parses string into a command structure
 *
 * @param string parseme - raw command string to be parsed
 * @param size_t length - length of command (parseme)
 * @returns struct - command structure
 */
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
     */

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

    /* may not have exec yet, if no match was found e.g */
    parseexec(cmd, parseme, parseme + length - 1);

    return cmd;
}

/**
 * Runs command
 *
 * @param struct c - command to be run
 * @returns int - return code of child that is run or error
 */
int
runcmd(struct cmd *c)
{
    pid_t pid;
    int status;
    builtin *builtin_cmd;

    if (!bin_isset_flag(c->flags, EXEC)) return 0; // nothing to do
    if (!c->argv) return -1;

    /* check if it is a builtin */
    builtin_cmd = get_builtin(c->argv[0]);
    if (builtin_cmd) {
        return builtin_cmd->execute(c); /* run that if so */
    }

    /*
    @ref https://man7.org/linux/man-pages/man2/fork.2.html
    @ref from Graham's lab 4/5 - https://loop.dcu.ie/mod/book/view.php?id=2054177&chapterid=433265
    */
    pid = fork();
    if (pid == 0) { /* child */
        if (bin_isset_flag(c->flags, REDRI)) dup2(c->fdin, STDIN_FILENO);
        if (bin_isset_flag(c->flags, REDRO)) dup2(c->fdout, STDOUT_FILENO);

        /* set parent=<path to shell executable> */
        setenv("parent", path_to_shell, 0);

        /* @ref https://man7.org/linux/man-pages/man3/exec.3.html */
        execvp(c->argv[0], c->argv);
        perror(c->argv[0]);
        cleancmd(c); /* cleanup as exec does not replace proccess */
        exit(127); /* child exits */
        /* posix standard sais 127 is used when command is not found
         * @ref 2.8.2 Exit Status for Commands
         * https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html
         */
    } else {
        c->pid = pid; /* set child pid */
        if (!bin_isset_flag(c->flags, BACK)) { // run in background
            waitpid(pid, &status, 0);
            c->rc = status;
        } /* else don't wait, run in background */
    }

    return 0;
}

#define print_if_true(cond, ...) if (cond) printf(__VA_ARGS__);

/**
 * Outputs command to console (good for debugging)
 *
 * @param struct c - command to print
 */
void
printcmd(struct cmd *c)
{
    /* print the flags */
    printf("Command @%p\n", (void *)c);
    printf("| pid: %d\n", c->pid);
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

    printf("| argc: ");
    print_if_true(bin_isset_flag(c->flags, EXEC), "%d\n", c->argc);

    printf("| argv: ");
    if (bin_isset_flag(c->flags, EXEC) && c->argv) {
        vec_print(char *, c->argv, c->argc, "%s");
    } else {
        printf("(nil)\n");
    }

}

/**
 * Cleans the command, i.e. closes open file descriptors and frees up any
 * memory
 *
 * @param struct c - command to clean
 * @returns int - return code
 */
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

/**
 * Reads command from file stream into a buffer. Also draws prompt if
 * in interactive mode
 *
 * @param int mode - read/execute mode (CMDINTER or CMDBATCH)
 * @param file stream - stream to read from
 * @param string buf - buffer to write read bytes to
 * @param size_t bufsz - size of buffer
 * @param size_t *bytes_read - pointer to bytes_read, caller should pass
 *                             writable address, this will store how many
 *                             bytes were read from stream
 * @return int - return code
 */
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

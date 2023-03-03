#include <stdio.h>
#include "parser.h"
#include "lib.h"
#include "command.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

/* Main commands
 * - getcmd: 
 *      reads user input/batch line
 * - parsecmd:
 *      parses cmd into exec tree
 * - runcmd:
 *      runs the parsed command
 */

#define SZ 1024

void handle_interrupt(int sigint);
void shell();

static int mode;
static FILE *stream;
static struct cmd *cmd;

int
main(int argc, char **argv)
{
    /*
     char buf[];
     while (getcmd(buf, sz)) {
        command = parsecmd(buf)
        runcmd(command);
     }
     */
    /* Test token matcher
    char *line = "echo 'hello'&<inputfile>>outputfile";
    int length = 37;
    parser_parse_line(line, length);
    */

    /*
    size_t length;
    size_t capacity;
    char *line = "echo 'hello' < infile";
    char *end = line;
    while (*++end);


    char **command = parseargs(line, end, &length, &capacity);
    vec_print(char *, command, length, "%s");
    */

    /*
    char line[] = "echo    'hello'     &    >    outfile";

    struct cmd *c = parsecmd(line, strlen(line));
    printcmd(c);
    runcmd(c);
    cleancmd(c);
    printf("This is after run!");
    */

    mode = CMDINTER;
    if (argc > 1) {
        stream = fopen(argv[1], "r");
        if (stream == NULL) {
            perror(argv[1]);
            return 1;
        }
        mode = CMDBATCH;
    }

    shell();
    return 0;
}

void
shell()
{
    char line[SZ];
    size_t bytes_read;
    int cmd_is_valid;

    cmd = NULL;
    signal(SIGINT, handle_interrupt);

    /* TODO strip single arg strings whitespace, e.g. "echo " */
    cmd_is_valid = getcmd(mode, stream, line, SZ, &bytes_read) == 0;
    while (cmd_is_valid) {
        cmd = parsecmd(line, bytes_read);
        printcmd(cmd);
        runcmd(cmd);
        cleancmd(cmd);
        cmd = NULL;
        cmd_is_valid = getcmd(mode, stream, line, SZ, &bytes_read) == 0;
    }
}

void
handle_interrupt(int sigint)
{
    puts("");
    if (cmd) {
        kill(cmd->pid, SIGINT);
        printf("killed [%d]\n", cmd->pid);
        fflush(stdout);
        cleancmd(cmd);
        cmd = NULL;
    }
    shell();
}

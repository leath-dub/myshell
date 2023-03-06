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
#include <stdlib.h>

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
char *path_to_shell;

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
    path_to_shell = argv[0];
    setenv("shell", path_to_shell, 0);
    setenv("prompt", "%p> ", 0); /* set default prompt */

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
    int end_of_file;

    cmd = NULL;
    signal(SIGINT, handle_interrupt);

    /* TODO strip single arg strings whitespace, e.g. "echo " */
    end_of_file = feof(stdin);
    cmd_is_valid = getcmd(mode, stream, line, SZ, &bytes_read) == 0;
    while (!end_of_file && cmd_is_valid) {
        cmd = parsecmd(line, bytes_read);

#       ifdef debug
            printcmd(cmd);
#       endif

        runcmd(cmd);
        cleancmd(cmd);
        cmd = NULL;
        end_of_file = feof(stdin);
        cmd_is_valid = getcmd(mode, stream, line, SZ, &bytes_read) == 0;
    }
}

void
handle_interrupt(int sigint)
{
    puts("");
    system("stty echo icanon"); /* pause disables line buffering, so we make
                                   sure it doesn't keep echo off and cannonical
                                   mode on */
    if (cmd) {
        kill(cmd->pid, SIGINT);
        printf("killed [%d]\n", cmd->pid);
        fflush(stdout);
        cleancmd(cmd);
        cmd = NULL;
    }
    shell();
}

#include <stdio.h>
#include "parser.h"
#include "lib.h"
#include "builtin.h"
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
int shell();

static int mode;
static FILE *stream;
static struct cmd *cmd;
char *path_to_shell;

int
main(int argc, char **argv)
{
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

    return shell();
}

int
shell()
{
    char line[SZ];
    size_t bytes_read;
    int cmd_is_valid;
    int end_of_file;
    int run_status;
    int temp;

    cmd = NULL;
    signal(SIGINT, handle_interrupt);

    end_of_file = feof(stdin);
    cmd_is_valid = getcmd(mode, stream, line, SZ, &bytes_read) == 0;
    while (!end_of_file && cmd_is_valid) {
        cmd = parsecmd(line, bytes_read);

#       ifdef debug
            printcmd(cmd);
#       endif

        run_status = runcmd(cmd);
        if (run_status == exit_quit) {
            fprintf(stderr, "exited [%d]\n", cmd->rc);
            temp = cmd->rc;
            cleancmd(cmd);
            // exit(cmd->rc); /* propagate return code */
            return temp;
        }

        cleancmd(cmd);
        cmd = NULL;
        end_of_file = feof(stdin);
        cmd_is_valid = getcmd(mode, stream, line, SZ, &bytes_read) == 0;
    }

    return 0;
}

void
handle_interrupt(int sigint)
{
    puts("");
    system("stty echo icanon"); /* pause disables line buffering, so we make
                                   sure it doesn't keep echo off and cannonical
                                   mode on */

    if (cmd) {
        kill(cmd->pid, SIGINT); /* kill child */
        printf("killed [%d]\n", cmd->pid);
        signal(SIGINT, handle_interrupt); /* reset the signal handler */
        return;
    }

    shell(); /* else redraw if empty */
}

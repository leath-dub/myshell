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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
-- Note -- I heavily use manpages as documentation, I try to reference
            them where necessary.

   Nameing convention for core functions, getcmd, parsecmd and runcmd come
   from the xv6 shell https://github.com/ag6288/XV6-Shell-Implementation/blob/master/shell.c
   This was demonstrated in this video https://www.youtube.com/watch?v=ubt-UjcQUYg
*/

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

/**
 * Main - entry point, sets environment and 
 *
 * @returns int return code
 */
int
main(int argc, char **argv)
{
    path_to_shell = argv[0];

    /* @ref https://man7.org/linux/man-pages/man3/setenv.3.html */
    setenv("shell", path_to_shell, 0);
    setenv("prompt", "%p $ ", 0); /* set default prompt */

    mode = CMDINTER;
    if (argc > 1) {
        stream = fopen(argv[1], "r");
        if (stream == NULL) {
            perror(argv[1]); /* @ref https://man7.org/linux/man-pages/man3/perror.3.html */
            return 1;
        }
        mode = CMDBATCH;
    }

    return shell();
}

/**
 * Runs commandline - reads input, parses it and executes based on parsed
 * command.
 *
 * @returns int return code
 */
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
    if (signal(SIGINT, handle_interrupt) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    /* @ref https://man7.org/linux/man-pages/man3/stdio.3.html */
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
            temp = cmd->rc; /* store status code before cmd struct is freed */
            cleancmd(cmd);
            return temp;
        }

        cleancmd(cmd);
        cmd = NULL;
        end_of_file = feof(stdin);
        cmd_is_valid = getcmd(mode, stream, line, SZ, &bytes_read) == 0;
    }

    return 0;
}

/**
 * Handler for SIGINT (^C), if child is running, kill it, otherwise
 * reset command line
 *
 * @param int sigint - unix signal
 */
void
handle_interrupt(int sigint)
{
    puts("");

    /* @ref https://man7.org/linux/man-pages/man1/stty.1.html */
    system("stty echo icanon"); /* pause builtin disables line buffering, so we make
                                   sure it doesn't keep echo off and cannonical
                                   mode on */

    if (cmd) {
        kill((pid_t)cmd->pid, (int)SIGINT); /* kill child */
        printf("killed [%d]\n", cmd->pid);
        signal(SIGINT, handle_interrupt); /* reset the signal handler */
        return;
    }

    shell(); /* else redraw if empty */
}

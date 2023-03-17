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
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

/* because we are using ansi C, setenv is platform dependent
 * change this to your platform
 */
int setenv(const char *name, const char *value, int overwrite);
/* realpath is also platform dependent */
char *realpath(const char *restrict path, char *restrict resolved_path);
/* dprintf also platform dependent */
int dprintf(int fd, const char *restrict format, ...);

#include "builtin.h"
#include "token.h"
#include "lib.h"
#include "parser.h"
#include "manual.h"

extern char **environ;
extern char path_to_shell[PATH_MAX];

builtin builtins[] = {
    {"cd", builtin_cd},
    {"clr", builtin_clr},
    {"dir", builtin_dir},
    {"environ", builtin_environ},
    {"echo", builtin_echo},
    {"help", builtin_help},
    {"pause", builtin_pause},
    {"quit", builtin_quit},
    {0, 0},
};

/**
 * Returns builtin associated with name provide or null
 *
 * @param string builtin_name - query name of builtin
 * @returns struct - reference to associated builtin
 */
builtin *
get_builtin(char *builtin_name)
{
    int match_found;

    for (builtin *builtin_iterator = builtins; *(int *)builtin_iterator; builtin_iterator += 1) {
        match_found = !strcmp(builtin_iterator->name, builtin_name);
        if (match_found) {
            return builtin_iterator;
        }
    }

    return NULL;
}

/**
 * Cd command changes current directory to first argument or prints current
 * if no argument provided
 *
 * @param struct cmd - command
 * @returns int - status code
 */
int
builtin_cd(struct cmd *cmd)
{
    int argc;
    char **argv;
    char *directory;
    int argument_given;
    int directory_ok;
    int failed_to_change_directory;
    int path_invalid;
    char resolved_path[PATH_MAX];
    const int overwrite = 1;

    argc = cmd->argc;
    argv = cmd->argv;

    argument_given = argc > 1;
    if (!argument_given) {
        dprintf(cmd->fdout, "current: %s\n", getenv("PWD"));
        cmd->rc = 0;
        return 0;
    }

    directory = argv[1];
    directory_ok = access(directory, R_OK) == 0;
    if (!directory_ok) {
        perror(directory);
        cmd->rc = 1;
        return 1;
    }

    path_invalid = realpath(directory, resolved_path) == NULL;
    if (path_invalid) {
        perror(directory);
        cmd->rc = 1;
        return 1;
    }

    setenv("OLDPWD", getenv("PWD"), overwrite);
    setenv("PWD", resolved_path, overwrite);

    failed_to_change_directory = chdir(resolved_path) == -1;
    if (failed_to_change_directory) {
        perror(resolved_path);
        cmd->rc = 1;
        return 1;
    }

    cmd->rc = 0;
    return 0;
}

/**
 * Clears the screen
 *
 * @param struct cmd - command
 * @returns int - status code
 */
int
builtin_clr(struct cmd *cmd)
{
    /* \033[2J tells terminal to clear screen
     * \033[H tells terminal to position cursor to home
     */
    dprintf(cmd->fdout, "\033[2J\033[H");

    cmd->rc = 0;
    return 0;
}

/**
 * Alias to ls -la, lists directory provided or current directory
 *
 * @param struct cmd - command
 * @returns int - status code
 */
int
builtin_dir(struct cmd *cmd)
{
    char **new_command;
    struct cmd temp_command;
    size_t length;
    size_t capacity;

    /* allocate space for new command */
    length = 0;
    capacity = 16;
    new_command = calloc(capacity, sizeof(char *));

    /* prefix command */
    vec_push_back(char *, new_command, capacity, length, "ls");
    vec_push_back(char *, new_command, capacity, length, "-la");
    vec_push_back(char *, new_command, capacity, length, "--color=always");

    /* copy the end as the old command */
    for (int i = 1; i < cmd->argc; i++) {
        vec_push_back(char *, new_command, capacity, length, cmd->argv[i]);
    }
    vec_push_back(char *, new_command, capacity, length, 0); // null terminate

    /* use old cmd as a template for temp_command */
    memmove(&temp_command, cmd, sizeof(struct cmd));
    temp_command.argv = new_command; /* set new argv */
    temp_command.argc = length;
    runcmd(&temp_command); /* execute */
    free(new_command);

    return 0;
}

/**
 * Outputs environment variables to console
 *
 * @param struct cmd - command
 * @returns int - status code
 */
int
builtin_environ(struct cmd *cmd)
{
    char **environment_variables;

    environment_variables = environ;
    for (char **variable = environment_variables
            ; *variable
            ; variable += 1) {
        dprintf(cmd->fdout, "%s\n", *variable);
    }

    cmd->rc = 0;
    return 0;
}

/**
 * Output arguments newline separated
 *
 * @param struct cmd - command
 * @returns int - status code
 */
int
builtin_echo(struct cmd *cmd)
{
    int is_last;
    char *argument;
    const char separator = ' ';

    for (int i = 1; i < cmd->argc; i += 1) {
        argument = cmd->argv[i];
        dprintf(cmd->fdout, "%s", argument);

        is_last = i == cmd->argc - 1;
        if (is_last) {
            break;
        }
        write(cmd->fdout, &(char[]){separator, 0}, 1);
    }
    write(cmd->fdout, "\n", 1); // finally print newline

    cmd->rc = 0;
    return 0;
}

/**
 * Outputs help info
 *
 * @param struct cmd - command
 * @returns int - status code
 *
 * @ref: https://www.geeksforgeeks.org/pipe-system-call/
 * this helped a lot
 */
int
builtin_help(struct cmd *cmd)
{
    pid_t pid;
    int is_child;
    int status;
    int manual_page_pipe[2];
    int manual_writer;
    int manual_reader;

    pipe(manual_page_pipe);
    manual_reader = manual_page_pipe[0];
    manual_writer = manual_page_pipe[1];

    /* fork more with that temp file as argument */
    pid = fork();
    is_child = pid == 0;
    if (is_child) {
        /* we still need to allow output redirection */
        if (bin_isset_flag(cmd->flags, REDRO)) dup2(cmd->fdout, STDOUT_FILENO);

        close(manual_writer); /* child only needs reader */
        dup2(manual_reader, STDIN_FILENO);
        close(manual_reader); /* no need anymore */

        /* set parent=<path to shell executable> */
        setenv("parent", path_to_shell, 0);
        execvp("more", (char *[]){"more", NULL});
        perror("more");
        exit(1); /* child exits */
    } else {
        close(manual_reader); /* child needs reader */
        write(manual_writer, __manual_readme_md, __manual_readme_md_len);
        close(manual_writer);

        cmd->pid = pid;
        waitpid(pid, &status, 0); /* wait for child */
        fflush(stdin);
        cmd->rc = status;
    }

    return 0;
}

/**
 * Pauses execution until user presses enter
 *
 * @param struct cmd - command
 * @returns int - status code
 */
int
builtin_pause(struct cmd *cmd)
{
    char char_from_user;
    const char *input_message = "Press enter to continue...";


    dprintf(cmd->fdout, "%s", input_message);

    /* below sets canonical mode in the terminal
     * this disables input buffering so we can
     * take characters as the user enters them */
    system("stty -echo -icanon");
    loop {
        if (read(cmd->fdin, &char_from_user, 1) == -1) {
            perror("read");
            system("stty echo icanon"); /* disable cannocial mode */
            return -1;
        }
        if (char_from_user == '\n' && char_from_user != EOF) {
            system("stty echo icanon"); /* disable cannocial mode */
            break;
        }
    }
    write(cmd->fdout, "\n", 1);

    cmd->rc = 0;
    return 0;
}

/**
 * Exits the shell gracefully
 *
 * @param struct cmd - command
 * @returns int - status code
 */
int
builtin_quit(struct cmd *cmd)
{
    cmd->rc = 0;
    return exit_quit; /* exit_quit is an integer outside of range of a
                         8 bit process exit status, just allows the
                         caller to distinguish between failed program
                         execution and request to quit */
}

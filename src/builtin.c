#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include <spawn.h>

#include "builtin.h"
#include "token.h"
#include "lib.h"
#include "parser.h"

/*
 * TODO: add io redirection here
 */

extern char **environ;

builtin builtins[] = {
    {"cd", builtin_cd},
    {"clr", builtin_clr},
    {"dir", builtin_dir},
    {"environ", builtin_environ},
    {"echo", builtin_echo},
    {"help", 0},
    {"pause", builtin_pause},
    {"quit", builtin_quit},
    {0, 0},
};

builtin *
get_builtin(char *builtin_name)
{
    builtin *builtin_iterator;
    int match_found;

    builtin_iterator = builtins;
    for (builtin *builtin_iterator = builtins; *(int *)builtin_iterator; builtin_iterator += 1) {
        match_found = !strcmp(builtin_iterator->name, builtin_name);
        if (match_found) {
            return builtin_iterator;
        }
    }

    return NULL;
}

int
builtin_cd(struct cmd *cmd)
{
    int argc;
    char **argv;
    char *directory;
    int argument_given;
    int directory_ok;
    char resolved_path[PATH_MAX];
    const int overwrite = 1;

    argc = cmd->argc;
    argv = cmd->argv;

    argument_given = cmd->argc > 1;
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

    /* TODO: handle errors */
    realpath(directory, resolved_path);
    setenv("OLDPWD", getenv("PWD"), overwrite);
    setenv("PWD", resolved_path, overwrite);
    chdir(resolved_path);

    cmd->rc = 0;
    return 0;
}

int
builtin_clr(struct cmd *cmd)
{
    dprintf(cmd->fdout, "\033[2J\033[H");

    cmd->rc = 0;
    return 0;
}

int
builtin_dir(struct cmd *cmd)
{
    char **new_command;
    struct cmd temp_command;
    size_t length;
    size_t capacity;
    int any_arguments;

    any_arguments = cmd->argc > 1;
    if (!any_arguments) {
        /* print the current path if no arguments */
        dprintf(cmd->fdout, "current: %s\n", getenv("PWD"));
        cmd->rc = 0;
        return 0;
    }

    /* allocate space for new command */
    length = 0;
    capacity = 16;
    new_command = calloc(capacity, sizeof(char *));

    /* prefix command */
    vec_push_back(char *, new_command, capacity, length, "ls");
    vec_push_back(char *, new_command, capacity, length, "-la");

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
    write(cmd->fdout, "\n", 1);

    cmd->rc = 0;
    return 0;
}

// int builtin_help(struct cmd *cmd);

int
builtin_pause(struct cmd *cmd)
{
    char char_from_user;
    size_t input_message_length;
    const char *input_message = "Press enter to continue...";


    dprintf(cmd->fdout, "%s", input_message);

    system("stty -echo -icanon");
    loop {
        if (read(cmd->fdin, &char_from_user, 1) == 1) {
        }
        if (char_from_user == '\n' && char_from_user != EOF) {
            system("stty echo icanon");
            break;
        }
    }
    write(cmd->fdout, "\n", 1);

    cmd->rc = 0;
    return 0;
}

int
builtin_quit(struct cmd *cmd)
{
    cmd->rc = 0;
    return exit_quit;
}


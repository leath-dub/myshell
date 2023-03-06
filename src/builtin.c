#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <linux/limits.h>
#include <unistd.h>

#include "builtin.h"
#include "token.h"
#include "lib.h"

extern char **environ;

builtin builtins[] = {
    {"cd", builtin_cd},
    {"clr", builtin_clr},
    {"dir", builtin_dir},
    {"environ", builtin_environ},
    {"echo", builtin_echo},
    {"help", 0},
    {"pause", builtin_pause},
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

    argument_given = argc > 1;
    if (!argument_given) {
        printf("current: %s\n", getenv("PWD"));
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
    printf("\033[2J\033[H");
    cmd->rc = 0;
    return 0;
}

int
builtin_dir(struct cmd *cmd)
{
    int argc;
    char **argv;
    char *directory;
    size_t directory_length;
    int argument_given;
    int directory_ok;
    char *run;
    const char *error_message = "usage: dir <directory>";
    const char *prefix_command = "ls -la ";
    int prefix_command_length;

    argc = cmd->argc;
    argv = cmd->argv;

    argument_given = argc > 1;
    if (!argument_given) {
        fprintf(stderr, "%s\n", error_message);
        cmd->rc = 1;
        return 0;
    }

    directory = argv[1];
    directory_ok = access(directory, R_OK) == 0;
    if (!directory_ok) {
        perror(directory);
        cmd->rc = 1;
        return 1;
    }

    directory_length = strlen(directory);
    prefix_command_length = strlen(prefix_command);
    run = calloc(directory_length + prefix_command_length + 1, sizeof(char));
    memmove(run, prefix_command, prefix_command_length);
    memmove(run + prefix_command_length, directory, directory_length);

    system(run);

    cmd->rc = 0;
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
        printf("%s\n", *variable);
    }
    fflush(stdout);

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
        printf("%s", argument);

        is_last = i == cmd->argc - 1;
        if (is_last) {
            break;
        }
        putchar(separator);
    }
    putchar('\n');
    fflush(stdout);

    cmd->rc = 0;
    return 0;
}

// int builtin_help(struct cmd *cmd);

int
builtin_pause(struct cmd *cmd)
{
    char char_from_user;
    const char *input_message = "Press enter to continue...";


    printf("%s", input_message);
    fflush(stdout);

    system("stty -echo -icanon");
    loop {
        if (read(STDIN_FILENO, &char_from_user, 1) == 1) {
        }
        if (char_from_user == '\n') {
            system("stty echo icanon");
            break;
        }
    }
    puts("");

    cmd->rc = 0;
    return 0;
}

#ifndef BUILTIN_H
#define BUILTIN_H

#include "command.h"

typedef int (*builtin_func) (struct cmd *);

typedef struct {
    const char *name;
    builtin_func execute;
} builtin;


builtin *get_builtin(char *builtin_name);
int builtin_cd(struct cmd *cmd);
// int builtin_clr(struct cmd *cmd);
int builtin_dir(struct cmd *cmd);
// int builtin_environ(struct cmd *cmd);
// int builtin_echo(struct cmd *cmd);
// int builtin_help(struct cmd *cmd);
// int builtin_pause(struct cmd *cmd);

#endif

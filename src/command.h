#include <stdint.h>
#include <stdio.h>
#include <aio.h>

#ifndef COMMAND_H
#define COMMAND_H

/*
 * parseargs -> parse slice to argument vector
 * getrhs -> parse slice to next special char or newline
 */


/* Modes that define whether getcmd treats input from user, or raw */
#define CMDBATCH 0x1
#define CMDINTER 0x2 // interactive mode draws a prompt

/* command functionality flags */
#define EXEC 0x1
#define BACK 0x2
#define REDRI 0x4
#define REDRO 0x8

/*
struct cmd {
    uint8_t flags;
    char *fsin; // TODO change thes to be FILE *
    char *fsout;
    char *modein;
    char *modeout;
    int argc;
    char **argv;
};
*/


struct cmd {
    uint8_t flags;
    int fdout;
    int fdin;
    int argc;
    int rc;
    pid_t pid;
    char **argv;
};

int parseio(struct cmd *c, char *start, char *match, size_t match_length, char *end, int flags, int type);
int parsewrite(struct cmd *c, char *start, char *match, size_t match_length, char *end);
int parseread(struct cmd *c, char *start, char *match, size_t match_length, char *end);
int parseappend(struct cmd *c, char *start, char *match, size_t match_length, char *end);
int parseback(struct cmd *c, char *start, char *match, size_t match_length, char *end);
int getcmd(int mode, FILE *stream, char *buf, size_t bufsz, size_t *read);
struct cmd *parsecmd(char *parseme, size_t length);
int runcmd(struct cmd *c);
void printcmd(struct cmd *c);
int parseexec(struct cmd *c, char *start, char *end);
int *getcmdfd(struct cmd *c, int type);
int cleancmd(struct cmd *c);

typedef int (*tok_fn)(struct cmd *, char *, char *, size_t, char *);

#endif

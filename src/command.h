#include <stdint.h>
#include <stdio.h>

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
#define REDI 0x4

typedef struct {
    size_t sz;
    char data[];
} rawcmd;

typedef struct {
    uint8_t flags;
    char *fsin;
    char *fsout;
    char *modein;
    char *modeout;
    int argc;
    char **argv;
} cmd;

rawcmd *getcmd(int mode);
cmd *parsecmd(rawcmd *parseme);
int runcmd(cmd *c);

#endif

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

#include <stdint.h>
#include <aio.h>
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

int getcmd(int mode, FILE *stream, char *buf, size_t bufsz, size_t *read);
struct cmd *parsecmd(char *parseme, size_t length);
int runcmd(struct cmd *c);
void printcmd(struct cmd *c);
int parseexec(struct cmd *c, char *start, char *end);
int cleancmd(struct cmd *c);

typedef int (*tok_fn)(struct cmd *, char *, char *, size_t, char *);

#endif

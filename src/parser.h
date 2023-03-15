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

#ifndef PARSER_H
#define PARSER_H
#include "command.h"

void parser_parse_line(char *raw_input, int length);
char **parseargs(char *start, char *end, size_t *_length, size_t *_capacity);
int parseio(struct cmd *c, char *start, char *match, size_t match_length, char *end, int flags, int type);
int parsewrite(struct cmd *c, char *start, char *match, size_t match_length, char *end);
int parseread(struct cmd *c, char *start, char *match, size_t match_length, char *end);
int parseappend(struct cmd *c, char *start, char *match, size_t match_length, char *end);

/* strings */
#define STR_DOUBLE_QUOTE_OPEN 0x1
#define STR_SINGLE_QUOTE_OPEN 0x2

#endif

#ifndef PARSER_H
#define PARSER_H

void parser_parse_line(char *raw_input, int length);
char **parseargs(char *start, char *end);

#endif

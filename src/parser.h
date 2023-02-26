#ifndef PARSER_H
#define PARSER_H

static const char IFS = ' ';

void parser_parse_line(char *raw_input, int length);
char **parseargs(char *start, char *end, size_t *_length, size_t *_capacity);

#endif

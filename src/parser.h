#ifndef PARSER_H
#define PARSER_H

void parser_parse_line(char *raw_input, int length);
char **parseargs(char *start, char *end, size_t *_length, size_t *_capacity);

/* strings */
#define STR_DOUBLE_QUOTE_OPEN 0x1
#define STR_SINGLE_QUOTE_OPEN 0x2

#endif

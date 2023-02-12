#include <stdio.h>
#include "parser.h"

int
main(int argc, char **argv)
{
    char *line = "echo 'hello'&<inputfile>>outputfile";
    int length = 37;
    parser_parse_line(line, length);
    return 0;
}

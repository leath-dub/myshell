#include <stdio.h>
#include "parser.h"
#include "lib.h"

/* Main commands
 * - getcmd: 
 *      reads user input/batch line
 * - parsecmd:
 *      parses cmd into exec tree
 * - runcmd:
 *      runs the parsed command
 */

int
main(int argc, char **argv)
{
    /*
     char buf[];
     while (getcmd(buf, sz)) {
        command = parsecmd(buf)
        runcmd(command);
     }
     */
    /* Test token matcher
    char *line = "echo 'hello'&<inputfile>>outputfile";
    int length = 37;
    parser_parse_line(line, length);
    */

    char *line = "echo 'hello' < infile";
    // find the '<', this will be the end ptr
    char *end = line;
    while (*++end != '<');


    char **command = parseargs(line, end);
    return 0;
}

#include <stdio.h>
#include "parser.h"
#include "lib.h"
#include "command.h"
#include <string.h>

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

    /*
    size_t length;
    size_t capacity;
    char *line = "echo 'hello' < infile";
    char *end = line;
    while (*++end);


    char **command = parseargs(line, end, &length, &capacity);
    vec_print(char *, command, length, "%s");
    */
    // char line[] = "echo -n 'hello' & > outfile";
    char line[] = "echo    'hello'     &    >    outfile";

    struct cmd *c = parsecmd(line, strlen(line));
    printcmd(c);
    runcmd(c);
    cleancmd(c);
    printf("This is after run!");
    return 0;
}

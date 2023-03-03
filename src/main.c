#include <stdio.h>
#include "parser.h"
#include "lib.h"
#include "command.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Main commands
 * - getcmd: 
 *      reads user input/batch line
 * - parsecmd:
 *      parses cmd into exec tree
 * - runcmd:
 *      runs the parsed command
 */

#define SZ 1024

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

    /*
    char line[] = "echo    'hello'     &    >    outfile";

    struct cmd *c = parsecmd(line, strlen(line));
    printcmd(c);
    runcmd(c);
    cleancmd(c);
    printf("This is after run!");
    */

    FILE *fp;
    int mode;
    int cmd_is_valid;

    fp = NULL;
    mode = CMDINTER;
    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror(argv[1]);
            return 1;
        }
        mode = CMDBATCH;
    }

    char line[SZ];
    struct cmd *cmd;

    /* TODO strip single arg strings whitespace, e.g. "echo " */
    while (getcmd(mode, fp, line, SZ) == 0) {
        cmd = parsecmd(line, SZ);
        printcmd(cmd);
        runcmd(cmd);
        cleancmd(cmd);
    }

    return 0;
}

#include <stdlib.h>
#include <stdio.h>

void
die(char *msg)
{
    perror(msg);
    exit(1);
}

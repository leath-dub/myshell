#include <stdint.h>

#ifndef COMMAND_H
#define COMMAND_H

typedef struct {
    uint32_t args_l;
    char *args[];
} command_t;

#endif

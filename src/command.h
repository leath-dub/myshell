#include <stdint.h>

typedef struct {
    uint32_t args_l;
    char *args[];
} command_t;

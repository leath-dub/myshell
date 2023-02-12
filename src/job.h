#include <sys/types.h>

#include "command.h"

typedef enum {
    job_foreground, job_background
} job_status_t;

/* The jid will be used to identify jobs from the shell */
typedef struct {
    uint32_t jid;
    pid_t pid;
    job_status_t status;
    uint8_t code; // exit status of job
    command_t cmd; /* command_t has a VLA, when a job is made we over allocate
    The struct to allow for the command size */
} job_t;

job_t *job_new(char **cmd, uint32_t cmd_l);
void job_background_lhs(job_t *lhs, char *raw_input, uint32_t pos);

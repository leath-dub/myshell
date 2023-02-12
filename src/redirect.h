#include <stdint.h>
#include "job.h"

void redirect_stdin(job_t *lhs, char *raw_input, uint32_t pos);
void redirect_stdout(job_t *lhs, char *raw_input, uint32_t pos);
void redirect_stdin_append(job_t *lhs, char *raw_input, uint32_t pos);

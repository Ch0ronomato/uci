#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

#ifndef ENGINE_H
#define ENGINE_H
// will handle the forking and execing.
void process_job(job_t *job);
#endif

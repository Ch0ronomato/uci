#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "engine.h"

int main(int argc, string* argv, string *env) {
   int read;
   size_t len = 0;
   string line;
   while((read = getline(&line, &len, stdin)) != -1) {
   		job_t *job = parse(len, line);
   		process_job(job, env);
		free(job);
   }

   return 0;
}


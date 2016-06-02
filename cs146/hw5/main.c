#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "engine.h"

int main(int argc, string* argv, string *env) {
   int read;
   size_t len = 0;
   string line;
   if (argc == 1) {
       printf("? ");
       fflush(stdout);
   }
   FILE *fp = argc == 1 ? stdin : fopen(argv[argc -1], "r");
   while((read = getline(&line, &len, fp)) != -1) {
   		job_t *job = parse(len, line);
   		process_job(job, env);
		free(job);
		if (argc == 1) {
			printf("? ");
		}
   }

   return 0;
}


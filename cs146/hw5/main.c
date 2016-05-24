#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "engine.h"

void process(job_t *job);
int main(int argc, string* argv) {
   ssize_t read;
   size_t len = 0;
   string line;
   while((read = getline(&line, &len, stdin)) != -1) {
   		job_t *job = parse(len, line);
   		process_job(job);
		free(job);
   }

   return 0;
}

void process(job_t *job) {
	// for now, we're just printing. 
	int i, j;
	printf("%d ", job->task_count);
	for (i = 0; i < job->task_count; i++) {
		if (job->tasks[i].redirect & 1) {
			printf("< %s ", job->tasks[i].inputname);
			free(job->tasks[i].inputname);
		}
		printf("%s ", job->tasks[i].cmd);
		free(job->tasks[i].cmd);
		for (j = 0;	j < job->tasks[i].flag_size; j++) {
			printf("%s ", job->tasks[i].flags[j]);
			free(job->tasks[i].flags[j]);
		}
		for (j = 0;	j < job->tasks[i].arg_size; j++) {
			printf("%s ", job->tasks[i].args[j]);
			free(job->tasks[i].args[j]);
		}
		if (job->tasks[i].redirect & 2) {
			printf("> %s ", job->tasks[i].outputname);
			free(job->tasks[i].outputname);
		}
		if (i + 1 < job->task_count) printf(" | ");
	}
	printf("\n");
}

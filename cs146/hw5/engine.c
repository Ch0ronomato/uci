#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "engine.h"
#include "parser.h"

typedef char* string;

/**
 * Method will fill buf with strings that are
 * ready for execvpe
 */
void prepare(task_t *task, string *buf);
void process_job(job_t *job) {
	int i;
	for (i = 0; i < job->task_count; i++) {
		if (fork() && wait()) {
			execv(job->tasks[i].cmd);
		} else {
			printf("Resuming parent");	
		}
	}
}

void prepare(task_t *task, string *buf) {
	printf("prepare");
	exit(-1);
}

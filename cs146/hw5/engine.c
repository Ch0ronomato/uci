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
/**
 * @TODOS as of 25
 *	1. get pipes working
 *	2. get redirects working
 *	3. do extras
 */
void process_job(job_t *job, string *env) {
	int i,j;
	for (i = 0; i < job->task_count; i++) {
		// prepare all tasks.
		string *args = malloc(sizeof(string) * (1 + job->tasks[i].flag_size + job->tasks[i].arg_size));
		args[0] = malloc(sizeof(char) * strlen(job->tasks[i].cmd));
		args[0] = job->tasks[i].cmd; // for safety
		prepare(&job->tasks[i], args);
		if (fork() && wait(NULL)) {
			printf("Resuming parent");	
		} else {
			printf("Child");
			execvpe(job->tasks[i].cmd, args, env);
			printf("Shit...");
			exit(-1);
		}
	}
}

void prepare(task_t *task, string *buf) {
	// buf should be the correct size here.
	int i = 0;
	for (; i < task->flag_size; i++) {
		buf[i + 1] = malloc(sizeof(char) * strlen(task->flags[i]));
		strcpy(buf[i + 1], task->flags[i]);
	}
	int temp = i;
	for (; i < task->flag_size + task->arg_size; i++) {
		buf[i + 1] = malloc(sizeof(char) * strlen(task->args[i - temp]));
		strcpy(buf[i + 1], task->args[i - temp]);
	}	
}

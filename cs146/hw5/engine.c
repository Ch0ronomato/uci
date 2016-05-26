#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "engine.h"
#include "parser.h"

/**
 * Method will fill buf with strings that are
 * ready for execvpe
 */
void prepare(task_t *task, string *buf);

/**
 * Method will handle spawning a process (essentially the fork and execr)
 */
void forkandexec(task_t *t, string *env);
/**
 * @TODOS as of 25
 *	1. get pipes working
 *	2. get redirects working
 *	3. do extras
 */
void process_job(job_t *job, string *env) {
	int i,j;
	if (job->task_count > 1) {
		// we have a pipe
	} else {
		forkandexec(&job->tasks[0], env);
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

void forkandexec(task_t *t, string *env) {
	// prepare all tasks.
	string *args = malloc(sizeof(string) * (1 + t->flag_size + t->arg_size));
	args[0] = malloc(sizeof(char) * strlen(t->cmd));
	args[0] = t->cmd; // for safety
	prepare(t, args);
	if (fork() && wait(NULL)) {
		printf("Resuming parent");	
	} else {
		printf("Child");
		execvpe(t->cmd, args, env);
		printf("Shit...");
		exit(-1);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "engine.h"
#include "parser.h"

#define REDIR_INPUT 1
#define REDIR_OUTPUT 2
#define REDIR_APPEND 4
/**
 * Method will fill buf with strings that are
 * ready for execvpe
 */
void prepare(task_t *task, string *buf);

/**
 * Method is a wrapper of prepare
 */
void getargs(task_t *task, string **buf);

/**
 * Method will handle spawning a process (essentially the fork and execr)
 */
void forkandexec(task_t *t, string *env, int background);

/**
 * Method will handle a pipe. For now only two.
 * When doing #6, this will have to change.
 * @TODO: #6
 */
void handlepipe(job_t *job, string *envp);

/**
 * Method will handle redirects.
 * The redirect member variable on the task
 * is a bit vector of three bits.
 * 1 = input
 * 2 = output
 * 4 = output append
 */
void setupnshredirects(task_t *task);
/**
 * @TODOS as of 25
 *	3. do extras
 */
void process_job(job_t *job, string *env) {
	if (job->task_count == 0) return;
	for (int i = 0; i < job->task_count; i++) {
		if (!strcmp(job->tasks[i].cmd, "exit")) exit(0);
	}
	if (job->task_count > 1) {
		int id;
		if ((id = fork())) {
			if (!job->background) {
				waitpid(id, NULL, 0);
				fflush(stdout);
			}
		} else {
			handlepipe(job, env);
		}
	} else {
		forkandexec(&job->tasks[0], env, job->background);
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
	buf[i + 1] = NULL;
}

void getargs(task_t *t, string **buf) {
	buf[0] = malloc(sizeof(string) * (1 + t->flag_size + t->arg_size));
	buf[0][0] = malloc(sizeof(char) * strlen(t->cmd));
	buf[0][0] = t->cmd; 
	prepare(t, buf[0]);
}

void forkandexec(task_t *t, string *env, int background) {
	// prepare all tasks.
	string *args;
	getargs(t, &args);
	if (fork()) {
		if (!background) {
			wait(NULL);
		}
	} else {
		setupnshredirects(t);
		execvpe(t->cmd, args, env);
		printf("%s not found\n", t->cmd);
		exit(-1);
	}
}

void handlepipe(job_t *job, string *envp) {
	// we have a pipe
	int fds[MAX_PIPE][2];
	string *args;
	char buf[BUFSIZ];
	for (int i = 0; i < job->task_count - 1; i++) {
		pipe(fds[i]);
	}
	if (fork()) {
		// main guy.
		dup2(fds[0][1], fileno(stdout));
		// close the rest.
		for (int i = 0; i < job->task_count - 1; i++) {
			close(fds[i][0]); close(fds[i][1]);
		}
		getargs(&job->tasks[0], &args);
		setupnshredirects(&job->tasks[0]);
		execvpe(job->tasks[0].cmd, args, envp);
		exit(-5);
	} else {
		int id, parent=(job->task_count-2) ? 0 : 1;
		for (id = 1; id < job->task_count-1; id++) {
			if (fork()) {
				// parent will keep going
			} else {
				break;
			}
		}
		parent = id == (job->task_count - 1);
		if (!parent) {
			dup2(fds[id-1][0], fileno(stdin)); // input to output
			dup2(fds[id][1], fileno(stdout)); // output to input
			for (int i = 0; i < job->task_count - 1; i++) {
				close(fds[i][0]); close(fds[i][1]);
			}
			getargs(&job->tasks[id], &args);
			setupnshredirects(&job->tasks[id]);
			execvpe(job->tasks[id].cmd, args, envp);
			exit(-6);
		} else {
			dup2(fds[id-1][0], fileno(stdin));
			for (int i = 0; i < job->task_count - 1; i++) {
				close(fds[i][0]); close(fds[i][1]);
			}
			getargs(&job->tasks[id], &args);
			setupnshredirects(&job->tasks[id]);
			execvpe(job->tasks[id].cmd, args, envp);
			exit(-7);
		}
		exit(0);
	}
}

void setupnshredirects(task_t *task) {
	// error check.
	// redirect would be 7 or 6 too cause this error
	if (task->redirect >= (REDIR_OUTPUT | REDIR_APPEND)) {
		perror("Cannot handle redirect output and append together");
		exit(-1); // this is okay, i'll be in a fork.
	}

	if (task->redirect & REDIR_INPUT) {
		FILE *fp = fopen(task->inputname, "r");
		dup2(fileno(fp), fileno(stdin));
		fclose(fp);
	} 

	if (task->redirect & REDIR_OUTPUT) { 
		FILE *fp = fopen(task->outputname, "w");
		dup2(fileno(fp), fileno(stdout));
		fclose(fp);
	} else if (task->redirect & REDIR_APPEND) {
		FILE *fp = fopen(task->outputname, "a");
		dup2(fileno(fp),fileno(stdout));
		fclose(fp);
	}
}

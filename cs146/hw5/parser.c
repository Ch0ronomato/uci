#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define STATE_START_HOLD 0
#define STATE_START 1
#define STATE_CMD_HOLD 2
#define STATE_CMD 3
#define STATE_ARGS_HOLD 4
#define STATE_ARGS 5

void resize_job_size(job_t *jobs, size_t old_size, size_t new_size) {
	job_t *newjob = malloc(sizeof(job_t));
	newjob->tasks = malloc(new_size);
	memset(newjob->tasks, 0, new_size);
	memcpy(newjob->tasks, jobs->tasks, old_size);
	free(jobs->tasks);
	free(jobs);
	jobs = newjob;
}

int validate_state(int current) {
	switch (current) {
		case STATE_START_HOLD:
			return STATE_START;
		case STATE_CMD_HOLD:
			return STATE_CMD;
		case STATE_ARGS_HOLD:
			return STATE_ARGS;
		default:
			return current;
	}
}
void copy_cmd(task_t *current_task, int bufsize, char buf[]) {
	current_task->cmd = malloc(sizeof(char) * bufsize);
	memcpy(current_task->cmd, buf, bufsize); // @TODO: Use strcpy
	memset(buf, 0, bufsize);
}
void copy_args(int bufsize, int *num_flags, int *num_args, char buf[], task_t *current_task) {
	string *sink = buf[0] == '-' ? &(current_task->flags[0]) : &(current_task->args[0]);
	int *count = buf[0] == '-' ? num_flags : num_args;
	sink[*count] = malloc(sizeof(char) * bufsize);
	memcpy(sink[(*count)++], buf, bufsize);
	memset(buf, 0, bufsize);
}
/**
 * parse 
 *	Method will take in a line of input
 *	and return a pathalogical concrete
 *	syntax tree. Each element in the 
 *	array can be piped to the next
 *	item in the list without worry.
 */
job_t *parse(ssize_t len, string line) {
	// begin character by character
	// parse.
	int bufsize=0, state = STATE_START_HOLD, num_args = 0, num_flags = 0;
	char *p = &(line[0]) - sizeof(char), buf[BUFSIZ];
	task_t *current_task;

	// allocate our objects
	job_t *job = malloc(sizeof(job_t));
	job->tasks = malloc(sizeof(task_t));
	current_task = &(job->tasks[0]);
	current_task->flags = malloc(sizeof(char*) * BUFSIZ);
	current_task->args = malloc(sizeof(char*) * BUFSIZ);
	while(p++ != &(line[strlen(line)-1])) {
		switch(*p) {
			case ' ':
			case '\n':
				// we have hit a space.
				if (state == STATE_START) {
					// copy the buffer into our name.
					copy_cmd(current_task, bufsize, buf);
					state = STATE_CMD_HOLD;
					bufsize = 0;
				} else if (state == STATE_CMD || state == STATE_ARGS) {
					// copy the buffer into the appropiate place.
					// essentially this verbose line is equivalent to state -= 1
					copy_args(bufsize, &num_flags, &num_args, buf, current_task);
					state = state == STATE_CMD ? STATE_CMD_HOLD : STATE_ARGS_HOLD;
					bufsize = 0;
				}
				break;
			case '<':
				current_task->redirect = -1;	
				break;
			case '>':
				current_task->redirect = 1;
				break;
			case '|':
				// we have hit a pipe!
				break;
			default:
				// just fill the buffer.
				buf[bufsize++] = *p;
				state = validate_state(state);
				break;
		}
	}

	// If the buffer size is greater than zero, we need this last piece of data.
	if (bufsize) {
		if (state == STATE_START) {
			copy_cmd(current_task, bufsize, buf);
		} else if (state == STATE_CMD) {
			copy_args(bufsize, &num_flags, &num_args, buf, current_task);
		} else if (state == STATE_ARGS) {

		}
	}
	printf("The command we've read is: %s\n", current_task->cmd);
	int i;
	for (i = 0; i < num_flags; i++){ printf("%d:%s\n", i, current_task->flags[i]);}
	for (i = 0; i < num_args; i++){ printf("%d:%s\n", i, current_task->args[i]);}
	return job;
}

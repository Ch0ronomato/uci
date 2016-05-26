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
#define STATE_REDIRECT_HOLD 6
#define STATE_REDIRECT 7

int validate_state(int current) {
	switch (current) {
		case STATE_START_HOLD:
			return STATE_START;
		case STATE_CMD_HOLD:
			return STATE_CMD;
		case STATE_ARGS_HOLD:
			return STATE_ARGS;
		case STATE_REDIRECT_HOLD:
			return STATE_REDIRECT;
		default:
			return current;
	}
}
void copy_cmd(task_t *current_task, int bufsize, char buf[]) {
	current_task->cmd = malloc(sizeof(char) * bufsize);
	strcpy(current_task->cmd, buf); 
	memset(buf, 0, bufsize);
}

void copy_args(int bufsize, int *num_flags, int *num_args, char buf[], task_t *current_task) {
	string *sink = buf[0] == '-' ? &(current_task->flags[0]) : &(current_task->args[0]);
	int *count = buf[0] == '-' ? num_flags : num_args;
	sink[(*count)] = malloc(sizeof(char) * bufsize);
	strcpy(sink[(*count)++], buf);
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
job_t *parse(int len, string line) {
	// begin character by character
	// parse.
	int bufsize=0, state = STATE_START_HOLD, num_args = 0, num_flags = 0, redir = 0, jobsize = 1;
	char *p = &(line[0]) - sizeof(char), buf[BUFSIZ];
	task_t *current_task;

	// allocate our objects
	job_t *job = malloc(sizeof(job_t));
	current_task = &(job->tasks[0]);
	current_task->redirect = 0;
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
				} else if (state == STATE_REDIRECT && redir) {
					string *sink = redir == 1 ?  &(current_task->inputname) : &(current_task->outputname);
					// only take the first one
					sink[bufsize] = '\0';
					if (!(current_task->redirect & redir)) {
						current_task->redirect |= redir;
						sink[0] = malloc(sizeof(char) * bufsize);
						strcpy(sink[0], buf);
						memset(buf, 0, bufsize);
					}
					redir = 0;
					bufsize = 0;
				}
				break;
			case '<':
			case '>':
				redir = (*p == '<') ? 1 : 2;	
				state = STATE_REDIRECT_HOLD;
				break;
			case '|':
				// we have hit a pipe!
				current_task->flag_size = num_flags;
				current_task->arg_size = num_args;
				jobsize++;
				if (jobsize >= MAX_PIPE) {
					perror("Too many pipes were allocated");
					exit(-1);
				}
				current_task = &(job->tasks[jobsize - 1]);
				current_task->redirect = 0;
				bufsize=0, state = STATE_START_HOLD, num_args = 0, num_flags = 0, redir = 0;
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
		} else if (state == STATE_CMD || state == STATE_ARGS) {
			copy_args(bufsize, &num_flags, &num_args, buf, current_task);
		} 	
	}
	current_task->flag_size = num_flags;
	current_task->arg_size = num_args;
	job->task_count = jobsize;
	return job;
}

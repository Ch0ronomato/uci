#include <stdio.h>
#include <stdlib.h>

#ifndef PARSER_H
#define PARSER_H
#define ARG_FLAG_SIZE 16
#define MAX_PIPE 255
typedef char* string;
typedef struct task_s {
  int redirect;
  int flag_size;
  int arg_size;
  string cmd;
  string flags[ARG_FLAG_SIZE];
  string args[ARG_FLAG_SIZE];
  string outputname;
  string inputname;
} task_t;

typedef struct job_s {
	int task_count;
	task_t tasks[MAX_PIPE];
} job_t;

job_t *parse(ssize_t len, char *input);
#endif

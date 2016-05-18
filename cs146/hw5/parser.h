#include <stdio.h>
#include <stdlib.h>

typedef char* string;
typedef struct task_s {
  string cmd;
  string *flags;
  string *args;
  int redirect;
  string outputname;
  string inputname;
} task_t;

typedef struct job_s {
	int task_count;
	task_t *tasks;
} job_t;

job_t *parse(ssize_t len, char *input);

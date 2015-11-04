// Ian Schweer, 22514022
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3

typedef struct threadargs_s {
	int *begin;
	int *end;
	long tid;
	int sum;
	int min;
	int max;
} threadargs_t;

void *getAverage(void *args) {
	threadargs_t *data = (threadargs_t *)args;
	while (data->begin != data->end) {
		int val = *data->begin;
		data->sum += val; 
		data->begin++;
		if (val > data->max) {
			data->max = val; 
		} 
		if (val < data->min) {
			data->min = val; 
		}
	}
	if(data->tid != 4) pthread_exit(args); 
	return NULL;
}

threadargs_t *makeData(long id, int data[], int ops) {
	threadargs_t *arg = malloc(sizeof(threadargs_t));
	arg->tid = id + 1;
	arg->begin = &data[id * ops];
	arg->end = &data[(1 + id) * ops];
	arg->sum = 0;
	arg->min = BUFSIZ; 
	arg->max = -1 * BUFSIZ;
	return arg; 
}

int main() {
	char name[BUFSIZ];
	int data[BUFSIZ];
	int i = 0, j = 0, globmin, globmax;
	float avg = 0.0f;
	while (fgets(name, BUFSIZ, stdin) != NULL) {
		if (name[0] == '\n') continue;
		data[i++] = strtol(name, NULL, 10);
	}

	// make threads
	int opsPerThread = floor(i / (NUM_THREADS + 1));
	pthread_t threads[NUM_THREADS];
	threadargs_t *args[NUM_THREADS + 1];
	
	// joinablility in threads.
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	for (j = 0; j < NUM_THREADS; j++) {
		args[j] = makeData(j, data, opsPerThread);
		pthread_create(&threads[j], &attr, getAverage, (void *) args[j]); 
	}

	// do main thread.
	args[j] = makeData(j, data, opsPerThread);
	args[j]->end = &data[i];
	getAverage((void *) args[j]);
	avg += args[j]->sum;
	globmin = args[j]->min;
	globmax = args[j]->max;	

	// join and exit
	for (j = 0; j < NUM_THREADS; j++) {
		pthread_join(threads[j], NULL);
		avg += args[j]->sum;
		globmin = globmin > args[j]->min ? args[j]->min : globmin;
		globmax = globmax < args[j]->max ? args[j]->max : globmax;
		free(args[j]);
	}
	printf("Max:%i\nMin:%i\nAverage:%f", globmax, globmin, avg / i);

	// free my data
	return 0;
}

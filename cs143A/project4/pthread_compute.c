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
} threadargs_t;

void *getAverage(void *args) {
	threadargs_t *data = (threadargs_t *)args;
	while (data->begin != data->end) {
		data->sum += *data->begin;
		data->begin++;
	}
	return NULL;
}

threadargs_t *makeData(long id, int data[], int ops) {
	threadargs_t *arg = malloc(sizeof(threadargs_t));
	arg->tid = id + 1;
	arg->begin = &data[id * ops];
	arg->end = &data[(1 + id) * ops];
	arg->sum = 0;
	return arg; 
}

int main() {
	char name[BUFSIZ];
	int data[BUFSIZ];
	int i = 0, j = 0;
	float avg = 0.0f;
	while (fgets(name, BUFSIZ, stdin) != NULL) {
		data[i++] = strtol(name, NULL, 10);
	}

	// make threads
	int opsPerThread = floor(i / (NUM_THREADS + 1));
	pthread_t threads[NUM_THREADS];
	threadargs_t *args[NUM_THREADS + 1];
	for (j = 0; j < NUM_THREADS; j++) {
		args[j] = makeData(j, data, opsPerThread);
		pthread_create(&threads[j], NULL, getAverage, (void *) args[j]); 
	}

	// do main thread.
	args[j] = makeData(j, data, opsPerThread);
	args[j]->end = &data[i];
	getAverage((void *) args[j]);
	avg += args[j]->sum;

	// join and exit
	for (j = 0; j < NUM_THREADS; j++) {
		pthread_join(threads[j], NULL);
		avg += args[j]->sum;
	}
	printf("Average = %f", avg / i);
	return 0;
}

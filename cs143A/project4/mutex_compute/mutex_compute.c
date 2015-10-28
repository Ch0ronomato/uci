#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3

typedef struct threadargs_s {
	long tid;
	int sum;
	int count;
} threadargs_t;

pthread_mutex_t mutexfgets;
int bail = 0;

void *getAverage(void *args) {
	threadargs_t *data = (threadargs_t *)args;
	while(1) {
		if (bail) break;
		char name[BUFSIZ], *read;
		pthread_mutex_lock(&mutexfgets);
		read = fgets(name, BUFSIZ, stdin);
		pthread_mutex_unlock(&mutexfgets);
		if (read == NULL) {
			bail = 1;
			break;
		}
		// sum our number.
		int input = strtol(name, NULL, 10);
		if (errno == ERANGE) {
			bail = 1;	
			break;
		}
		data->sum += input;
		data->count++;
	}
	if(data->tid != 4) pthread_exit(args); 
	return NULL;
}

threadargs_t *makeData(long id) {
	threadargs_t *arg = malloc(sizeof(threadargs_t));
	arg->tid = id + 1;
	arg->sum = 0;
	arg->count = 0;
	return arg; 
}

int main() {
	int j = 0, i = 0;
	float avg = 0.0f;
	pthread_mutex_init(&mutexfgets, NULL);

	// make threads
	pthread_t threads[NUM_THREADS];
	threadargs_t *args[NUM_THREADS + 1];
	
	// joinablility in threads.
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	for (j = 0; j < NUM_THREADS; j++) {
		args[j] = makeData(j);
		pthread_create(&threads[j], &attr, getAverage, (void *) args[j]); 
	}

	// do main thread.
	args[j] = makeData(j);
	getAverage((void *) args[j]);
	avg += args[j]->sum;
	i += args[j]->count;

	// join and exit
	for (j = 0; j < NUM_THREADS; j++) {
		pthread_join(threads[j], NULL);
		avg += args[j]->sum;
		i += args[j]->count;
		free(args[j]);
	}
	printf("Average = %f", avg /(double)i);
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutexfgets);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "que.h"
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

static ELE _que[QUE_MAX];
static int _front = 0, _rear = 0;
extern int producers_working;

static int matches = 0;

pthread_mutex_t m;
sem_t sem_full;
sem_t sem_empty;
void add_match()
{
    //Note: you will need to lock this update because it is a race condition
    pthread_mutex_lock(&m);
    matches++;
    pthread_mutex_unlock(&m);
}

void report_matches(char *pattern)
{
    printf("Found %d total matches of '%s'\n", matches, pattern);
}

int que_init()
{
	pthread_mutex_init(&m, NULL);
	sem_init(&sem_full, 0, 0);
	sem_init(&sem_empty, 0, 0);
}

void que_error(char *msg)
{
    fprintf(stderr, "***** Error: %s\n", msg);
    // exit(-1);
}

int que_is_full()
{
    return (_rear + 1) % QUE_MAX == _front; /* this is why one slot is unused */
}

int que_is_empty()
{
    return _front == _rear;
}

void que_enq(ELE v)
{
    if (que_is_full())
	sem_wait(&sem_empty);
    int max = 1000000, i = 0;
    while(que_is_full() && i < max)
	;
	// que_error("Enqueue on full queue");
    pthread_mutex_lock(&m);
    _que[_rear++] = v;
    if ( _rear >= QUE_MAX )
        _rear = 0;
    pthread_mutex_unlock(&m);
    if (que_is_full()) sem_post(&sem_full);
}

ELE que_deq()
{
    if (producers_working && que_is_empty()) { 
	struct timespec s;
	s.tv_sec = 1;
	s.tv_nsec = 1;
	int r;
	if ((r = sem_timedwait(&sem_full, &s)) == -1) {
		return (ELE){.string = " "};
	} 
    }
    int max = 10000000, i=0;
    while (que_is_empty() && i < max)
	;
    if (i == max) return (ELE){.string = " "};
    pthread_mutex_lock(&m);
    int x = _front;
    _front++;
    ELE ret = _que[x];
    _que[x].string[0] = '\0';
    if ( _front >= QUE_MAX )
        _front = 0;
    pthread_mutex_unlock(&m);
    if (que_is_empty()) 
    	sem_post(&sem_empty);
    return ret;
}


/*

int main()
{
    for ( int i=0; i<QUE_MAX-1; ++i )
    {
        Buffer b;
        sprintf(&b.string, "%d", i);
        que_enq(b);
    }
    while ( !que_is_empty() )
        printf("%s ", que_deq().string);
    putchar('\n');
}
*/

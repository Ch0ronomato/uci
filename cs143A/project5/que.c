#include <stdio.h>
#include <stdlib.h>
#include "que.h"


static ELE _que[QUE_MAX];
static int _front = 0, _rear = 0;
extern int producers_working;

static int matches = 0;

pthread_mutext_t m;

void add_match()
{
    //Note: you will need to lock this update because it is a race condition
    pthread_mutex_lock(m);
    matches++;
    pthread_mutex_unlock(m);
}

void report_matches(char *pattern)
{
    printf("Found %d total matches of '%s'\n", matches, pattern);
}

int que_init()
{
	pthread_mutex_init(&m, NULL);
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
    // replace this spin with something better.....
    while (que_is_full()) // note this is not right
        ;
    if ( que_is_full() )
        que_error("enq on full queue");
    _que[_rear++] = v;
    if ( _rear >= QUE_MAX )
        _rear = 0;
}

ELE que_deq()
{
    // replace this spin with something better.....
    while (producers_working && que_is_empty()) // note this is not right
        ;
    if ( que_is_empty() )
        que_error("deq on empty queue");
    ELE ret = _que[_front++];
    if ( _front >= QUE_MAX )
        _front = 0;
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

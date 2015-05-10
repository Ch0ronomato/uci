/* $begin shellmain */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csapp.h"
#include "mm.h"

#define MAXARGS   128

typedef enum {ALLOC, FREE, BLOCKLIST, HWRITE, HREAD, BESTFIT, FIRSTFIT, NA} options_t;
typedef struct memcmd_s {
	char *raw_command;
	char **args;
	int argc;
	options_t command;
} memcmd_t;

typedef struct heapblock_s heapblock_t;
struct heapblock_s {
	int id, size, allocated;
	char *bp;
	heapblock_t *next;
};

/* function prototypes */
memcmd_t init_command();
void eval(char *cmdline, heapblock_t *list);
int parseline(char *buf, char **argv, memcmd_t*);
void operate(memcmd_t metadata, heapblock_t *data);
void allocate(int size, heapblock_t *last); /* Allocates ${size} blocks */
int freeblock(int id); /* Frees the memory at id ${id} */
void blocklist(); /* Will print all blocks. Uses HDRP(), FTRP(), NEXTBLK() */
int writeheap(int id, char data, int n); /* Will write ${data} ${n} times at mem pos ${id} */
void printlist(int id, int n); /* Will print data at mem pos ${id} ${n} times */
int switchfit(int type); /* Will switch memory location algorithm. 0 is firstfit, 1 is bestfit */ 
static void *best_fit(size_t asize); /* Implementation of the best fit memory management algorithm */

memcmd_t init_command() {
	memcmd_t to_return;
	to_return.raw_command = NULL;
	to_return.args = NULL;
	to_return.argc = 0;
	to_return.command = NA;
	return to_return;
}

int main() {
    char cmdline[MAXLINE]; /* Command line */
    heapblock_t *list = malloc(sizeof(heapblock_t));
    list->id = last->size = last->allocated = 0;
    list->bp = NULL;
    list->next = NULL;
    while (1) {
		/* Read */
		printf("> ");                   
		Fgets(cmdline, MAXLINE, stdin); 
		if (feof(stdin))
		    exit(0);

		/* Evaluate */
		eval(cmdline, list);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline, heapblock_t *list) {
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    
    memcmd_t cmdobj = init_command();
    cmdobj.raw_command = cmdline;
    cmdobj.args = argv;

    strcpy(buf, cmdline);
    if (!parseline(buf, argv, &cmdobj)) { return; } // Don't do anything.
    operate(cmdobj, list);

    return;
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv, memcmd_t *rpath) {
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int valid = 1; 		 /* Return quit */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
		buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
		/* Replace the current index with the null terminator */
		/* so strcmp stops at it. */
		*delim = '\0'; 

		if (!strcmp(buf, "allocate")) { rpath->command = ALLOC; }
		else if (!strcmp(buf, "free")) { rpath->command = FREE; }
		else if (!strcmp(buf, "blocklist")) { rpath->command = BLOCKLIST; }
		else if (!strcmp(buf, "writeheap")) { rpath->command = HWRITE; }
		else if (!strcmp(buf, "printheap")) { rpath->command = HREAD; }
		else if (!strcmp(buf, "bestfit")) { rpath->command = BESTFIT; }
		else if (!strcmp(buf, "firstfit")) { rpath->command = FIRSTFIT; }
		else if (!strcmp(buf, "quit")) { printf("Quiting\n"); exit(0); }
		else { 
			if (rpath->command != NA) { /* Argument */
				rpath->args[rpath->argc] = malloc(sizeof(char));
				strcpy(rpath->args[rpath->argc++], buf);
			}
			else /* enter some error state. */
				valid = 0;
		}
		
		buf = delim + 1; /* progression step, next character. */
		while (*buf && (*buf == ' ')) /* Ignore spaces */
			buf++;
    }
    rpath->args[rpath->argc] = NULL;
    return valid;
}

void operate(memcmd_t metadata, heapblock_t *data) {
	heapblock_t *last = data;
	while(last->next != NULL) last = last->next;
	if (metadata.command == ALLOC) {
		if (metadata.argc != 1) printf("Bad arguments. Allocate needs 1 argument.\n");
		else allocate(atoi(metadata.args[0]), last);
	} else if (metadata.command == FREE) {
		if (metadata.argc != 1) printf("Bad arguments. Free needs 1 argument.\n");
		else freeblock(atoi(metadata.args[0]));
	} else if (metadata.command == BLOCKLIST) {
		if (metadata.argc != 0) printf("Bad arguments. Blocklist needs 0 arguments.\n");
		else blocklist();
	} else if (metadata.command == HWRITE) {
		if (metadata.argc != 3) printf("Bad arguments. Write block needs 3 arguments.\n");
		else writeheap(atoi(metadata.args[0]), *metadata.args[1], atoi(metadata.args[2]));
	}
}

void allocate(int size, heapblock_t *last) {
	heapblock_t hb;
	hb.size = size;
	hb.allocated = 0;
	hb.id = last->id + 1;
	hb.bp = (char*)mm_malloc(size);
}

int freeblock(int id) {
	return 0;
}

void blocklist() {

}

int writeheap(int id, char data, int size) {
	return 0;
}

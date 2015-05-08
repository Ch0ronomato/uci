/* $begin shellmain */
#include <stdio.h>
#include <string.h>
#include "csapp.h"
#include "mm.h"

#define MAXARGS   128

typedef enum {ALLOC, FREE, BLOCKLIST, HWRITE, HREAD, BESTFIT, FIRSTFIT, NA} options_t;
typedef struct memcmd_s
{
	char *raw_command;
	char *args;
	int argc;
	options_t command;
} memcmd_t;

/* function prototypes */
memcmd_t init_command();
void eval(char *cmdline);
int parseline(char *buf, char **argv, memcmd_t*);
int allocate(int size, int lastId); /* Allocates ${size} blocks and returns next id ({$lastId} + 1) */
int freeblock(int id); /* Frees the memory at id ${id} */
void blocklist(); /* Will print all blocks. Uses HDRP(), FTRP(), NEXTBLK() */
int writelist(int id, char data, int n); /* Will write ${data} ${n} times at mem pos ${id} */
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

int main() 
{
    char cmdline[MAXLINE]; /* Command line */
    while (1) {
		/* Read */
		printf("> ");                   
		Fgets(cmdline, MAXLINE, stdin); 
		if (feof(stdin))
		    exit(0);

		/* Evaluate */
		eval(cmdline);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    
    memcmd_t cmdobj = init_command();
    cmdobj.raw_command = cmdline;

    strcpy(buf, cmdline);
    if (!parseline(buf, argv, &cmdobj)) { return; } // Don't do anything.
	

    return;
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv, memcmd_t *rpath) 
{
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

		if (!strcmp(buf, "allocate")) { printf("Allocating\n"); }
		else if (!strcmp(buf, "free")) { printf("Freeing\n"); }
		else if (!strcmp(buf, "blocklist")) { printf("blocklist-ing\n"); }
		else if (!strcmp(buf, "writeheap")) { printf("writeheap-ing\n"); }
		else if (!strcmp(buf, "printheap")) { printf("printheap-ing\n"); }
		else if (!strcmp(buf, "bestfit")) { printf("Best fit\n"); }
		else if (!strcmp(buf, "firstfit")) { printf("First fit\n"); }
		else if (!strcmp(buf, "quit")) { printf("Quiting\n"); exit(0); }
		else { printf("Invalid command!\n"); valid = 0; } /* enter some error state. */
		
		buf = delim + 1; /* progression step, next character. */
		while (*buf && (*buf == ' ')) /* Ignore spaces */
			buf++;
    }
    argv[argc] = NULL;
    return valid;
}

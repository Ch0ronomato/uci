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
    parseline(buf, argv, &cmdobj); 
	
    return;
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv, memcmd_t *rpath) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
		buf++;

    /* Build the argv list */
    argc = 0;
  //   while ((delim = strchr(buf, ' '))) {
		// *delim = '\0';
		
		// if(strcmp(buf,"<")==0){
		// 	mode = PT_STDIN;
		// }else if(strcmp(buf,">")==0){
		// 	mode = PT_STDOUT;
		// }else if(strcmp(buf,"2>")==0){
		// 	mode = PT_STDERR;
		// }else{
		// 	switch(mode){
		// 	case PT_ARG:
		// 		argv[argc++] = buf;
		// 		break;
		// 	case PT_STDIN:
		// 		rpath->in = buf;
		// 		break;
		// 	case PT_STDOUT:
		// 		rpath->out = buf;
		// 		break;
		// 	case PT_STDERR:
		// 		rpath->err = buf;
		// 		break;
		// 	}
		// 	mode = PT_ARG;
		// }
		
		// buf = delim + 1;
		// while (*buf && (*buf == ' ')) /* Ignore spaces */
		// 	buf++;
  //   }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    return bg;
}

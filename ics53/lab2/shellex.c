/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

typedef struct redirects{
	char* in;
	char* out;
	char* err;
} RedirectionPaths;

RedirectionPaths init_r_paths(){
	RedirectionPaths rpath;
	rpath.in = NULL;
	rpath.out = NULL;
	rpath.err = NULL;
}

/* function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv, RedirectionPaths*);
int builtin_command(char **argv);

int redirect_stream(int, char*);

void bgreap_handle(int sig){
	int status;
	int id = wait(&status);
	printf("child reaped\nID: %d\nstatus: %d\n",id,status);
}


int main() 
{
    char cmdline[MAXLINE]; /* Command line */
	
    Signal(SIGCHLD, bgreap_handle);

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
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
	RedirectionPaths rpath = init_r_paths();	
	
    strcpy(buf, cmdline);
    bg = parseline(buf, argv, &rpath); 
	
	if(rpath.out != NULL)
		printf("stdout redirection: %s\n",rpath.out);
	
    if (argv[0] == NULL)  
		return;   /* Ignore empty lines */

    if (!builtin_command(argv)) { 
	if ((pid = Fork()) == 0) {   /* Child runs user job */
		if(rpath.in != NULL)
			redirect_stream(0,rpath.in);
		if(rpath.out != NULL)
			redirect_stream(1,rpath.out);
		if(rpath.err != NULL)
			redirect_stream(2,rpath.err);
	    if (execve(argv[0], argv, environ) < 0) {
		printf("%s: Command not found.\n", argv[0]);
		exit(0);
	    }
	}

	/* Parent waits for foreground job to terminate */
	if (!bg) {
	    int status;
	    if (waitpid(pid, &status, 0) < 0)
		unix_error("waitfg: waitpid error");
	}
	else
	    printf("%d %s", pid, cmdline);
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "quit")) /* quit command */
		exit(0);  
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
		return 1;
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
typedef enum {PT_ARG,PT_STDIN,PT_STDOUT,PT_STDERR} ParseMode;
int parseline(char *buf, char **argv, RedirectionPaths *rpath) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */
	
	ParseMode mode = PT_ARG;
	
    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
		buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
		*delim = '\0';
		
		if(strcmp(buf,"<")==0){
			mode = PT_STDIN;
		}else if(strcmp(buf,">")==0){
			mode = PT_STDOUT;
		}else if(strcmp(buf,"2>")==0){
			mode = PT_STDERR;
		}else{
			switch(mode){
			case PT_ARG:
				argv[argc++] = buf;
				break;
			case PT_STDIN:
				rpath->in = buf;
				break;
			case PT_STDOUT:
				rpath->out = buf;
				break;
			case PT_STDERR:
				rpath->err = buf;
				break;
			}
			mode = PT_ARG;
		}
		
		buf = delim + 1;
		while (*buf && (*buf == ' ')) /* Ignore spaces */
			buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    return bg;
}
/* $end parseline */

int redirect_stream(int stream, char* filename){
	int file;
	switch(stream){
	case 0:
		file = open(filename,O_RDONLY);
		break;
	case 1:
		file = open(filename,O_WRONLY|O_CREAT,0666);
		break;
	case 2:
		file = open(filename,O_WRONLY|O_CREAT,0666);
		break;
	default:
		return -1;
	}
	dup2(file,stream);
	close(file);
	return 0;
}

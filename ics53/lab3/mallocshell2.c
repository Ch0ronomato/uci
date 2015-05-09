
#include <stdio.h>
#include <string.h>
#include "csapp.h"
#include "mm.h"

#define CMD_BUFFER_SIZE 100

/* Might need a flag to remember which memory mode we're using.
	Ian thinks there might already be one in one of the class files
	but I haven't checked */
/* static int best_fit_flag = 0; */



/* next three functions are used by the main loop to parse input and make sure it is valid */

/* used by read_int to check if input is a number */
int is_int(char* str){
	int i = 0;
	if(str[0]=='\0')
		return 0;
	for(;str[i]!='\0';++i)
		if(str[i] < '0' || str[i] > '9')
			return 0;
	return 1;
}

/* reads input into buffer, then checks if input is a number.
	used for input parsing by main loop */
int read_int(char* buf){
	scanf("%s",buf);
	return is_int(buf);
}

/* reads input into buffer, then checks if input is a char.
	used for input parsing by main loop */
int read_char(char* buf){
	scanf("%s",buf);
	return buf[0] != '\0' && buf[1] == '\0';
}

/* handler for bad input */
void bad_param(){
	printf("bad parameters\n");
}

/* handler for bad input */
void bad_cmd(){
	printf("bad command\n");
}



/* Each command we're expected to implement has a cmd_x function associated with it,
	except for quit, which is handled in the main loop */

int cmd_allocate(int size){
	
	return 0;
}

int cmd_free(int blocknum){

	return 0;
}

int cmd_blocklist(){

	return 0;
}

int cmd_writeheap(int blocknum, char character, int times){

	return 0;
}

int cmd_printheap(int blocknum, int bytenum){

	return 0;
}

int cmd_bestfit(){

	return 0;
}

int cmd_firstfit(){

	return 0;
}

int main(){
	/* I figure that since we know beforehand exactly what inputs are acceptable,
		it would be easier to to use a buffer for each argument instead of a 
		complicated dynamic line-parse function */
	char cmd[CMD_BUFFER_SIZE], arg1[CMD_BUFFER_SIZE], arg2[CMD_BUFFER_SIZE], arg3[CMD_BUFFER_SIZE];
	
	/*
	each time through the loop:
	1.	we read the command token into the string cmd
	
	2.	if there are arguments associated with that command,
		we read them into the arg strings using read_int or
		read_char depending on the argument type we're checking for
		
	3.	if the read_x functions see the input is valid, the
		appropriate cmd_x function is called. else we print an
		error message with bad_param()
	*/
	while(1){
		printf("> ");
		scanf("%s",&cmd);
		if(strcmp(cmd,"allocate")==0){
			if(read_int(arg1))
				cmd_allocate(atoi(arg1));
			else
				bad_param();
		}else if(strcmp(cmd,"free")==0){
			if(read_int(arg1))
				cmd_free(atoi(arg1));
			else
				bad_param();
		}else if(strcmp(cmd,"blocklist")==0){
			cmd_blocklist();
		}else if(strcmp(cmd,"writeheap")==0){
			if(read_int(arg1)&&read_char(arg2)&&read_int(arg3))
				cmd_writeheap(atoi(arg1),arg2[0],atoi(arg3));
			else
				bad_param();
		}else if(strcmp(cmd,"printheap")==0){
			if(read_int(arg1)&&read_int(arg2))
				cmd_printheap(atoi(arg1),atoi(arg2));
			else
				bad_param();
		}else if(strcmp(cmd,"bestfit")==0){
			cmd_bestfit();
		}else if(strcmp(cmd,"firstfit")==0){
			cmd_firstfit();
		}else if(strcmp(cmd,"quit")==0){
			exit(0);
		}else{
			bad_cmd();
		}
	}
}
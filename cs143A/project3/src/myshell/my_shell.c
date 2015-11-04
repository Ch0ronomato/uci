// Ian Schweer
// 22514022
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void clearSpaces(char **word) {
	while (*word[0] == ' ' && *word[0] != '\n') *word+=1;	
}
int clearSpacesC(char **word) {
	int c = 0;
	while (*word[0] == ' ' && *word[0] != '\n') *word+=1, c++;
	return c;	
}

void processCmd(char *rawData, int n, char *env[]) {
	if (n == 0 && rawData == NULL) {
		exit(0);
	}
	// make sure we don't have spaces.
	rawData[n] = 0;
	clearSpaces(&rawData);
		
	// data is good, do stuff with it.
	char first[BUFSIZ], second[BUFSIZ], third[BUFSIZ], fourth[BUFSIZ]; // two args and flags
	int sanity = 0;
	while (rawData[sanity] != ' ' && rawData[sanity] != '-' && sanity < n) { sanity++; };
	rawData[sanity] = '\0';
	strcpy(first, rawData);

	rawData += sanity+1;
	if (sanity == n) {
		char *noargs[] = {first, NULL};
		execvpe(noargs[0], noargs, env);
	} else {
		// erase all spaces at the beginning.
		int remainder = n - sanity;
		clearSpaces(&rawData);
		if (rawData[0] == '-' || rawData[0] != ' ') {
			// handle the flag
			int pos = 0, i = 0;
			while (rawData[i] != ' ' && i < remainder) {
				second[i] = rawData[i];
				i++;
			}	
			rawData += i+1;
			remainder -= i + 1;
			remainder -= clearSpacesC(&rawData);
			if (rawData[0] == '\n' || rawData[0] == '\0') {
				char *args[] = {first, second, NULL};
				execvpe(args[0], args, env);
				printf("Could not execute command: %s\n", args[0]);
				exit(0);	
			}
			if (rawData[0] != ' ') {
				i = 0;
				pos++;
				while (rawData[i] != ' ' && i < remainder) {
					third[i] = rawData[i];
					i++;	
				}
				rawData += i + 1;
				remainder -= i + 1;
				remainder -= clearSpacesC(&rawData);
			} // argument three
			else {
				char *args[] = {first, second, NULL};
				execvpe(args[0], args, env);
				printf("Could not execute command: %s\n", args[0]);
				exit(0);	
			}

			if (rawData[0] != ' ' && rawData[0] != '\0') {
				i = 0;
				pos++;
				while(rawData[i] != ' ' && i < remainder) {
					fourth[i] = rawData[i];
					i++;
				}
				char *args[] = {first, second, third, fourth, NULL};
				execvpe(args[0], args, env);
				printf("Could not execute command: %s\n", args[0]);
				exit(0);	
			} // argument four
			else {
				char *args[] = {first, second, third, NULL};
				execvpe(args[0], args, env);
				printf("Could not execute command: %s\n", args[0]);
				exit(0);	
			}
		}
		else if (rawData[0] != ' ') {
				int i = 0, pos = 0;
				while (rawData[i] != ' ' && rawData[i] != '\n') {
					third[i] = rawData[i];
					i++;	
				}
				rawData += i + 1;
				clearSpaces(&rawData);
				if (rawData[0] != ' ') {
					i = 0;
					pos++;
					while(rawData[i] != ' ' && rawData[i] != '\n') {
						fourth[i] = rawData[i];
						i++;
					}
					char *args[] = {first, second, third, fourth, NULL};
					execvpe(args[0], args, env);
					printf("Could not execute command: %s\n", args[0]);
					exit(0);	
				} else {
					char *args[] = {first, second, third, NULL};
					execvpe(args[0], args, env);
					printf("Could not execute command: %s\n", args[0]);
					exit(0);	
				}
		}
		/*int c = 0, i = 0, pos = 0;
		char *args[BUFSIZ];
		rawData[n - sanity + 1] = ' ';
		s[pos++] = first;
		while (*(rawData + i) != '\n') {{
			char arg[BUFSIZ] = {'\0'};
			int j = 0;
			while (*(rawData + i) != ' ') {  
				*(arg + j) = *(rawData + i); 
				j++;
				i++;
			}
			*(arg + j) = '\0'; 
			strcpy(args[pos++], arg);
			i++;
		}}
		int a = 0;*/
	}
}

int main(int argc, char **argv, char *env[]) {
	char c = 'a', cmd[BUFSIZ];
	memset(cmd, '\0', sizeof(cmd));
	int i = 0;
	while (1) {
		printf("$: ");
		while (c != '\n' && c != EOF) {
			c = getchar();
			if (c == EOF) break;
			memcpy(&cmd[i++], &c, sizeof(char));
		}
		if (cmd[0] != '\0' && cmd[0] != '\n') {
			pid_t id = fork();
			if (id == 0 && c != EOF)
				processCmd(&cmd[0], i - 1, env);
			waitpid(id, NULL, 0);
		}
		if (c == EOF)
			exit(0);
		memset(cmd, 0, sizeof(cmd));
		i = 0;
		c = 'a';
	}
}

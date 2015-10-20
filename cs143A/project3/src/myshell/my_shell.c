#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

typedef enum states {
	CMD,
	ARGS
} states_t;

typedef struct state {
	states_t state;
	char data[BUFSIZ];
	int ready;
} state;

void clearSpaces(char **word) {
	while (*word[0] == ' ') *word+=1;	
}

void processCmd(char *rawData, int n, char *env[]) {
	if (n == 0 && rawData == NULL) {
		exit(0);
	}
	// make sure we don't have spaces.
	clearSpaces(&rawData);
	
	// data is good, do stuff with it.
	states_t currentState = CMD;
	state dataParsed[2];
	int sanity = 0;
	char args[BUFSIZ][BUFSIZ];
	while (rawData[sanity] != ' ' && rawData[sanity] != '-' && sanity < n) { sanity++; };
	memcpy(dataParsed[0].data, rawData, sanity);
	dataParsed[0].data[sanity] = '\0';

	rawData += sanity;
	if (sanity == n) {
		printf("CMD: %s\n", dataParsed[0].data);
		execvpe(dataParsed[0].data, NULL, env);
	} else {
		// erase all spaces at the beginning.
		clearSpaces(&rawData);
		memcpy(dataParsed[1].data, rawData, n - sanity);
		dataParsed[1].data[n - sanity] = '\0';
		printf("CMD: %s\nARG: %s\n", dataParsed[0].data, dataParsed[1].data);	

		char *follower = dataParsed[1].data;
		int i = 0, pos = 0, maxlen = n - sanity;
		// find the first character that is not a dash or space;
		while (dataParsed[1].data[i] != '-' && i < maxlen) i++;
		i = i == maxlen ? 0 : i+1;

		// get all our arguments seperate for execvpe
		while (i < maxlen) {
			if (follower[i] == ' ' || follower[i] == '-') {
				memcpy(args[pos], follower, i);
				follower += follower[i] == '-' ? i : i + 1;
				pos+=1;
				maxlen = i; 
				i = 0;
			}
			i++;
		}
		memcpy(args[pos++], follower, i);
		while (pos >= 0) {
			printf("args[%d]: %s\n", pos, args[pos]);
			pos--;
		}
	}
}

int main(int argc, char **argv, char *env[]) {
	printf("$: ");
	char c = 'a', cmd[BUFSIZ];
	int i = 0;
	while (c != '\n') {
		c = getchar();
		cmd[i++] = c;
	}
	cmd[i - 1] = '\0';
	processCmd(cmd, i - 1, env);
}

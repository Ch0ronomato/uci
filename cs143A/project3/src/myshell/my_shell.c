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

void processCmd(char *rawData, int n) {
	if (n == 0 && rawData == NULL) {
		exit(0);
	}
	// make sure we don't have spaces.
	while (rawData[0] == ' ') rawData+=1;
	
	// data is good, do stuff with it.
	states_t currentState = CMD;
	state dataParsed[2];
	int sanity = 0;
	while (rawData[sanity] != ' ' && rawData[sanity] != '-') { sanity++; };
	memcpy(dataParsed[0].data, rawData, sanity);
	dataParsed[0].data[sanity] = '\0';

	rawData += sanity;
	memcpy(dataParsed[1].data, rawData, n - sanity);
	dataParsed[1].data[n - sanity] = '\0';
	printf("CMD: %s\nARG: %s\n", dataParsed[0].data, dataParsed[1].data);	

	char args[BUFSIZ][BUFSIZ];
	char *follower = dataParsed[1].data;
	int i = 0, pos = 0, maxlen = n - sanity;
	// find the first character that is not a dash or space;
	while (dataParsed[1].data[i] != ' ' && dataParsed[1].data[i] != '-') i++;
	i++;

	// get all our arguments seperate for execvpe
	while (i < maxlen) {
		if (follower[i] == ' ' || follower[i] == '-') {
			memcpy(args[pos], follower, i);
			follower += i;
			pos+=1;
			maxlen = i; 
			i = 0;
		}
		i++;
	}
	memcpy(args[pos++], follower, i);
}

int main(int argc, char **argv) {
	printf("$: ");
	char c = 'a', cmd[BUFSIZ];
	int i = 0;
	while (c != '\n') {
		c = getchar();
		cmd[i++] = c;
	}
	cmd[i] = '\0';
	processCmd(cmd, i);
}

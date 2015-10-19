#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_CHILDREN 3

void printK(char c[], int k) {
	int i = 0;
	for (; i < k; i++) {
		putchar(c[0]);
	}
}

int main(int argc, char **argv) {
	int k = 10;
	char letters[] = {'A', 'B', 'C', 'D'};
	if (argc == 2) {
		// read in number of k.
		k = strtol(argv[1], NULL, 10); 
	}
	pid_t ids[NUM_CHILDREN];
	int i = 0;
	for (; i < NUM_CHILDREN; i++) {
		pid_t id = fork();
		if (id != 0) { 
			ids[i] = id;
		} // call parent	
		else {
			break;
		}
	}
	if (i == NUM_CHILDREN) {
		i = 0;
		for (; i < NUM_CHILDREN; i++)
			waitpid(ids[i], NULL, 0);
	}
	printK(letters + (i % 4), k);
	return 0;
}


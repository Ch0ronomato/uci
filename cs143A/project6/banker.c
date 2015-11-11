// Ian Schweer
// 22514022
// Simple implementation of bankers algorithm.
// Program only needs to determine the state of a resource allocation.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int arrayLessThan(int *X, int *Y, int P) {
	int i = 0;
	for (i = 0; i < P; i++)
		if (Y[i] < X[i])
			return 0;
	return 1;
}

void printWork(int R, int *work) {
	int i = 0;
	for(; i < R; i++)
		printf("%d\t", work[i]);
	printf("\n\n");
}

int findI(int P, int R, int need[][R], int *work, int *finished, int *ret) {
	int i = 0;
	*ret = -1;
	for (i = 0; i < P; i++) {
		if (finished[i] == 0 && arrayLessThan(need[i], work, R)) {
			*ret = i;
			printf("Process%d is good\n", i + 1);
			break;
		}
	}
	return i != P ? 1 : 0;
}

int safety(int P, int R, int max[][R], int alloc[][R], int *aval, int need[][R]) {
	// Assuming Work = Available.
	int work[R], totalalloc[R], finished[P], i = 0, j = 0; 
	memset(finished, 0, sizeof(finished));
	memset(work, 0, sizeof(finished));
	memset(totalalloc, 0, sizeof(totalalloc));
	// calculate the total allocated resources
	// onced we finished, read values
	
	// The actual amount of work is equal to the total available resources minus the amount
	// already allocated.  
	for (j = 0; j < R; j++)
		for (i = 0; i < P; i++)
			totalalloc[j] += alloc[i][j];

	for (i = 0; i < R; i++)
		work[i] = aval[i] - totalalloc[i];

	j = 0;
	while (1) {
		int val;
		if (findI(P, R, need, work, finished, &val)) {	
			finished[val] = 1;
			j++;
			printf("------work-------\n");
			printWork(R, work);
			printf("-----alloc-------\n");
			printWork(R, alloc[val]);
			for(i = 0; i < R; i++)
				work[i] += alloc[val][i];
		}		
		if (val == -1) break;	
	}
	return j == P ? 1 : 0;	
}

int main() {
	int P = 0, R = 0, i = 0, j = 0;
	// read in P, R
	scanf("%d", &P);
	scanf("%d", &R);
	if (P < 0) perror("No process count"), exit(-1);
	if (R < 0) perror("No resouce count"), exit(-1);
	int max[P][R], alloc[P][R], aval[R], need[P][R];	
	for (i = 0; i < R; i++)
		scanf("%d", &aval[i]);

	for (i = 0; i < P; i++)
		for (j = 0; j < R; j++)
			scanf("%d", &alloc[i][j]);

	for (i = 0; i < P; i++)
		for (j = 0; j < R; j++)
			scanf("%d", &max[i][j]);

	for (i=0; i < P; i++)
		for (j = 0; j < R; j++) 
			need[i][j] = max[i][j] - alloc[i][j];
	if (safety(P, R, max, alloc, aval, need)) {
		printf("The system is in a safe state\n");
	} else {
		printf("The system is in an unsafe state\n");
	}
	return 0;
}

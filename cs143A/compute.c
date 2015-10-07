#include <stdio.h>
#include <stdlib.h>

int getAverage(int data[], int datasize, int *max, int *min) {
	ssize_t readLines = 0;
	size_t len = 0;
	int count = 0, avg = 0;
	char *line;

	// initialize values, and run algorithm
	*max = -1;
	*min = 99999;
	for (int i = 0; i < datasize; i++) {
		int value = data[i];
		*max = *max < value ? value : *max;
		*min = *min > value ? value : *min;
		avg += value;
	}
	return avg / datasize;	
}

int main() {
	// read filename from stdin
	char name[100];
	int data[100];
	int i = 0, max, min, avg;
	while (fgets(name, 100, stdin) != NULL) {
		data[i++] = strtol(name, NULL, 10);
	}
	avg = getAverage(data, i, &max, &min);
	printf("max: %i\nmin: %i\naverage: %i", max, min, avg);
	return 0;
}

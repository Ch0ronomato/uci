#include <stdio.h>
#include <stdlib.h>

int main() {
	int c;
	while (EOF != (c = fgetc(stdin))) {
		printf("You entered %c\n", c);
	}
	return 0;
}

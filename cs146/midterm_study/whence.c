#include <stdio.h>

int main() {
	FILE *f = fopen("doc.txt", "r");
	int pos = fseek(f, -9, 2);
	printf("%d\t%c\n", pos, getc(f));
	return 0;
}

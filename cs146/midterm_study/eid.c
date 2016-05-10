#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	printf("uid=%d\teuid=%d\n", getuid(), geteuid());
	return 0;
}

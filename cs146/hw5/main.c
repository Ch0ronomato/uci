#include <stdio.h>
#include <stdlib.h>

typedef char* string;

int main(int argc, string* argv) {
   ssize_t read;
   size_t len = 0;
   string line;
   while((read = getline(&line, &len, stdin)) != -1) {
        printf("I got string %s\n", line);
   }

   return 0;
}

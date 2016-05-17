#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, string* argv) {
   ssize_t read;
   size_t len = 0;
   string line;
   while((read = getline(&line, &len, stdin)) != -1) {
   		parse(len, line);
   }

   return 0;
}

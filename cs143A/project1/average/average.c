/**
 * CS143a - Fall 2015
 * HW #1
 * @author: Kyle Benson
 *
 * This program reads numbers from a file specified as the only argument,
 * one number per line, and computes the average of all the numbers.
 * The result is printed out on the second line of output, with the first
 * line containing the filename and total number of values read.
 */

// What header file allows us to handle file I/O?
#include <stdio.h>
#include <stdlib.h> // This lets us call exit(...)

int main(int argc, char ** args)
{
  // First, get the filename from the command line arguments if specified
  if (argc < 2)
  {
    printf("Error: no input file specified\nUsage: average numbers.dat\n");
    exit(1);
  }
  char * filename = args[argc - 1];
  /* Use this line to verify you got the correct filename: */
  // printf("Reading numbers from file %s\n", filename);
  /**/

  // Open the file and verify it opened properly
  FILE * f = fopen(filename, "r");
  if (f == NULL)
  {
    printf("Failed to open file!\n");
    exit(2);
  }

  // Declare (and maybe initialize) any necessary variables before reading from the file
  char *line;
  float avg = 0;
  int count = 0;
  size_t len = 0;
  ssize_t linesRead = 0;

  // Read all the numbers from the file and compute the average
  while ((linesRead = getline(&line, &len, f)) != -1)
  {
    avg += strtol(line, NULL, 10);
    count++;
  }

  // Some error-checking if the file was empty
  if (count == 0)
  {
    printf("No numbers found in file!\n");
    exit(3);
  }

  // Lastly, print the result
  // NOTE: ensure that the actual average is on the SECOND line all by itself
  // or you may not receive credit for your program!
  printf("Average was \n%f\n", (avg / count));

  // SUCCESS!
  return 0;
}

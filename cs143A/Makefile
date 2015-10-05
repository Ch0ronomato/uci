CC = gcc
CFLAGS = -I.
PROGRAM = average
INPUT = numbers.dat

all: average.c
	$(CC) -o $(PROGRAM) average.c $(CFLAGS)

run: $(PROGRAM)
	./$(PROGRAM) $(INPUT)

clean:
	rm $(PROGRAM)

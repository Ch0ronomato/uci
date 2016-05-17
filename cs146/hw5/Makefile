FILES=parser.c main.c
OUTPUT=nsh
all:
	gcc -o $(OUTPUT) $(FILES)
debug:
	gcc -ggdb -o $(OUTPUT) $(FILES)
clean:
	rm nsh
	rm -rf $(OUTPUT).dSYM 2>/dev/null

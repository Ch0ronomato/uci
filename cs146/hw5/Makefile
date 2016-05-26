FILES=main.c parser.o engine.o
OUTPUT=nsh
all: obj/parser.o obj/engine.o
	gcc -o $(OUTPUT) $^ main.c
debug: parser-debug engine-debug
	gcc -ggdb -o $(OUTPUT) $(FILES)
clean: parser-clean engine-clean
	rm nsh
	rm -rf $(OUTPUT).dSYM 2>/dev/null
	rm -r obj

obj/parser.o: parser.c
	gcc -c $^
	mkdir obj 2>/dev/null || echo "dir exists. Skipping for now"
	mv parser.o obj

parser.dSYM: parser.c
	gcc -ggdb -c $^

parser-clean: 
	rm obj/parser.o

obj/engine.o:
	gcc -c $^
	mkdir obj 2>/dev/null || echo "dir exists. Skipping for now"
	mv engine.o obj

engine.dSYM: engine.c
	gcc -ggdb -c engine.c

engine-clean: 
	rm obj/engine.o

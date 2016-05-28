FILES=main.c parser.o engine.o
OUTPUT=nsh
GCC=gcc -std=gnu99
all: obj/parser.o obj/engine.o
	$(GCC) -o $(OUTPUT) $^ main.c
debug: parser.dSYM engine.dSYM
	$(GCC) -ggdb -o $(OUTPUT) $(FILES)
clean: parser-clean engine-clean
	rm nsh
	rm -rf $(OUTPUT).dSYM 2>/dev/null
	rm -r obj

obj/parser.o: parser.c
	$(GCC) -c $^
	mkdir obj 2>/dev/null || echo "dir exists. Skipping for now"
	mv parser.o obj

parser.dSYM: parser.c
	$(GCC) -ggdb -c $^
	mkdir obj 2>/dev/null || echo "dir exists. Skipping for now"
	mv parser.o obj

parser-clean: 
	rm obj/parser.o

obj/engine.o: engine.c
	$(GCC) -c $^
	mkdir obj 2>/dev/null || echo "dir exists. Skipping for now"
	mv engine.o obj

engine.dSYM: engine.c
	$(GCC) -ggdb -c engine.c
	mkdir obj 2>/dev/null || echo "dir exists. Skipping for now"
	mv engine.o obj

engine-clean: 
	rm obj/engine.o

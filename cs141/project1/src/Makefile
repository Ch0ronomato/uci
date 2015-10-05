EXECNAME=SIMPLESEM
OBJNAME=-o $(EXECNAME)
BUILD=g++
FILES=set.cpp halt.cpp jump.cpp jumpt.cpp main.cpp
all:
	$(BUILD) $(OBJNAME) $(FILES)

clean:
	rm SIMPLESEM

debug:
	$(BUILD) -ggdb $(OBJNAME) $(FILES)
run:
	./$(EXECNAME)

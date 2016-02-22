EXNAME=fs
DEPS=io/iosystem_impl.cpp filesystem/filesystem_impl.cpp main.cpp
OPTIONS=-std=c++11 -o
all:
	g++ $(OPTIONS) $(EXNAME) $(DEPS)
debug:
	g++ -ggdb $(OPTIONS) $(EXNAME) $(DEPS)
clean:
	rm $(EXNAME)

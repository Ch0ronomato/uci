DEBUG="vm.dSYM"
all:
	g++ -std=c++11 *.cpp -o vm
debug:
	g++ -std=c++11 *.cpp -ggdb -o vm
clean:
	rm vm
ifeq ($(wildcard "vm.dSYM/Contents/Info.plist"),)
	rm -rf vm.dSYM
endif

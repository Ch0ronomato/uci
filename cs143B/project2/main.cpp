#include <iostream>
#include <string>
#include <cassert>
#include "filesystem.h"
#include "io/iosystem.h"

int location = 1;
const int BLOCK_SIZE = 64;

void testCode();
int main() {
	FileSystem f;
	unsigned char buf[64];
	f.createFile("ian");
	f.openFile("ian");
	f.closeFile(0);
	f.destroyFile("ian");
	f.getIO()->read_block(7, buf);
	std::cout << (char *)buf << std::endl;
	return 0;
}
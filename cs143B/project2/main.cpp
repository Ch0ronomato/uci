#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include "filesystem.h"
#include "io/iosystem.h"

int location = 1;
const int BLOCK_SIZE = 64;

int main() {
	FileSystem f;
	unsigned char buf[64], buf2[64], buf3[64];
	f.createFile("ian");
	f.openFile("ian");
	f.writeFile(0, 'c', 64);
	f.closeFile(0);
	f.destroyFile("ian");
	f.getIO()->read_block(7, buf);
	f.getIO()->read_block(8, buf2);
	for (int i = 0; i < 64; i++)
		std::putchar(buf[i]);
	std::cout << std::endl;
	for (int i = 0; i < 64; i++)
		std::putchar(buf2[i]);
	std::cout << std::endl;
	// for (int i = 0; i < 64; i++)
	// 	std::putchar(buf3[i]);
	// std::cout << std::endl;
	return 0;
}
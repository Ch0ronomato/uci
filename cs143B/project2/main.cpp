#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include "filesystem/filesystem.h"
#include "io/iosystem.h"

int location = 1;
const int BLOCK_SIZE = 64;

int main() {
	IO_system *io = IO_system::CreateIOSystem();
	File_system *f = File_system::CreateFileSystem(io);
	unsigned char buf[66];
	f->create_file("ian");
	f->open_file("ian");
	f->write_file(0, 'a', 20);
	f->write_file(0, 'c', 42);
	f->write_file(0, 'd', 4);
	f->write_file(0, 'q', 50);
	f->write_file(0, 'e', 34);
	f->write_file(0, 'l', 40);
	f->seek_file(0, 0);
	f->read_file(0, 190);
	f->close_file(0);
	io->read_block(7, buf);
	for (int i = 0; i < 64; i++)
		std::putchar(buf[i]);
	std::cout << "\nFile being destoryed" << std::endl;
	// test destroying file
	f->destroy_file("ian");
	io->read_block(7, buf);
	for (int i = 0; i < 64; i++)
		std::putchar(buf[i]);
	// std::cout << std::endl;
	// for (int i = 0; i < 64; i++)
	// 	std::putchar(buf3[i]);
	// std::cout << std::endl;
	return 0;
}
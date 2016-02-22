// Ian schweer
// 22514022
#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include <sstream>
#include <vector>
#include "filesystem/filesystem.h"
#include "io/iosystem.h"

int location = 1;
const int BLOCK_SIZE = 64;

const std::string CREATE = "cr";
const std::string DESTORY = "de";
const std::string OPEN = "op";
const std::string CLOSE = "cl";
const std::string WRITE = "wr";
const std::string READ = "rd";
const std::string SEEK = "sk";
const std::string INIT = "in";
const std::string SAVE = "sv";
const std::string DIR = "dr";

std::vector<std::string> split_args(std::string input_args) {
	std::string buf;
	std::stringstream ss(input_args);
	std::vector<std::string> tokens;
	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}

std::vector<std::string> prompt() {
	std::string input;
	getline(std::cin, input);
	return split_args(input);
}

int string_to_int(std::string input) {
	return std::stoi(input);
}

void run() {
	// assume init.
	File_system *f = File_system::CreateFileSystem(IO_system::CreateIOSystem());
	std::string input;
	while (getline(std::cin, input)) {
		std::vector<std::string> params = split_args(input);
		if (params.size() == 0) {
			std::cout << std::endl;
			continue;
		}
		std::string cmd = params[0];
		if (!CREATE.compare(cmd)) {
			f->create_file(params[1]);
		} else if (!DESTORY.compare(cmd)) {
			f->destroy_file(params[1]);
		} else if (!OPEN.compare(cmd)) {
			f->open_file(params[1]);
		} else if (!CLOSE.compare(cmd)) {
			f->close_file(string_to_int(params[1]));
		} else if (!WRITE.compare(cmd)) {
			f->write_file(string_to_int(params[1]), params[2][0], string_to_int(params[3]));
		} else if (!READ.compare(cmd)) {
			f->read_file(string_to_int(params[1]), string_to_int(params[2]));
		} else if (!SEEK.compare(cmd)) {
			f->seek_file(string_to_int(params[1]), string_to_int(params[2]));
		} else if (!INIT.compare(cmd)) {
			if (params.size() == 1) {
				params.push_back("");
			}
			f->init(params[1], IO_system::CreateIOSystem());
		} else if (!SAVE.compare(cmd)) {
			f->save(params[1]);
		} else if (!DIR.compare(cmd)) {
			f->dir();
		}
		std::cout << std::endl;
	}
}

int main() {
	run();
	// IO_system *io = IO_system::CreateIOSystem();
	// File_system *f = File_system::CreateFileSystem(io);
	// f->create_file("ian");
	// f->create_file("ibn");
	// f->create_file("icn");
	// f->create_file("idn");
	// f->create_file("ien");
	// f->create_file("ifn");
	// f->create_file("ign");
	// f->create_file("ihn");

	// // @todo: This errors! I don't know why.
	// f->create_file("iin");
	// f->create_file("ijn");
	// f->create_file("ikn");
	// f->open_file("ian");
	// f->write_file(1, 'a', 20);
	// f->write_file(1, 'c', 42);
	// f->write_file(1, 'd', 4);
	// f->write_file(1, 'q', 50);
	// f->write_file(1, 'e', 34);
	// f->write_file(1, 'l', 40);
	// // f->init("temp.txt", IO_system::CreateIOSystem());
	// // f->open_file("ian");
	// f->seek_file(1, 0);
	// f->read_file(1, 190);
	// f->close_file(1);
	// f->open_file("ian");
	// f->close_file(1);
	// // io->read_block(7, buf);
	// f->save("temp.txt");

	// f->init("temp.txt", IO_system::CreateIOSystem());
	// f->open_file("ian");
	// f->read_file(1, 190);
	return 0;
}
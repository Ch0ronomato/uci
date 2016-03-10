#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include "pm.h"

typedef std::vector< std::vector< std::string> > file_content_t;
std::vector<std::string> split_args(std::string input_args) {
	std::string buf;
	std::stringstream ss(input_args);
	std::vector<std::string> tokens;
	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}

file_content_t get_file_as_string(std::ifstream &file) {
	std::vector< std::vector< std::string > > file_vector;
	if (file.is_open()) {
		std::string s;
		while (std::getline(file, s)) {
			file_vector.push_back(split_args(s));
		}
	}
	return file_vector;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cerr << "No input files provided" << std::endl;
		return -1;
	}

	
	// get the file content
	std::ifstream config_file(argv[1]), input_file(argv[2]);
	file_content_t config = get_file_as_string(config_file);
	std::vector< std::string > input = (get_file_as_string(input_file))[0];

	for (int i = 0; i < 2; i++) {
		std::string fname = "22514022" + std::to_string(i + 1) + ".txt";
		pm p(fname, i & 1);
		p.initialize(config);
		for (int i = 0; i < input.size(); i += 2) {
			int action = std::stoi(input[i]);
			int address = std::stoi(input[i + 1]);
			if (!action) {
				// read
				p.read(address);
			} else {
				// write
				p.write(address);
			}
		}
	}
	return 0;
}
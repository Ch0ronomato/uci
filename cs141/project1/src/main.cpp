#include "grammer.hpp"
#include "set.hpp"
#include "jump.hpp"
#include "jumpt.hpp"
#include "halt.hpp"
#include "statements.hpp"
#include "writer.hpp"
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::endl;
using std::ifstream;

Statement* parseLine(string line);
void sanitizeString(string& line);
void processLine(Statement *nonterm);
int main(int argc, char ** argv) {
	if (argc < 2) {
		cout << "Please run with filename (ex: SIMPLESEM file.S)" << endl;
		return 1;
	}
	string file(argv[argc - 1]);
	string line;
	ifstream ifs(file.c_str());

	// open file, read contents.
	int i = 0;
	if (ifs.is_open()) {
		while(ifs.good()) {
			std::getline(ifs, line);
			sanitizeString(line);
			cout << line << endl;
			cout << "---------------------" << endl;	
			processLine(parseLine(line));
			cout << "---------------------" << endl;	
		}
		ifs.close();
	}
	return 0;
}

Statement* parseLine(string line) {
	if (line.find("set") == 0 || line.find("SET") == 0) {
		return new Set(line);
	}
	else if (line.find("jumpt") == 0 || line.find("JUMPT") == 0) {
		return new Jumpt(line);
	}
	else if (line.find("jump") == 0 || line.find("JUMP") == 0) {
		return new Jump(line);
	}
	else if (line.find("halt") == 0 || line.find("HALT") == 0) {
		return new Halt(line);
	}
	else
		return NULL;
}

void sanitizeString(string& line) {
	if (line.find("\r") != string::npos) {
		// delete the last two characters.
		line.erase(line.find("\r"));
	}
}

void processLine(Statement *nonterm) {
	if (nonterm != NULL) {
		cout << "Statement" << endl;
		nonterm->parse();
		delete nonterm;
	}
}

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
void processLine(Statement *nonterm, Writer& w);
int main(int argc, char ** argv) {
	if (argc < 2) {
		cout << "Please run with filename (ex: SIMPLESEM file.S)" << endl;
		return 1;
	}
	string file(argv[argc - 1]);
	string outs = file + ".out";
	string line;
	ifstream ifs(file);
	Writer w(outs);

	// open file, read contents.
	int i = 0;
	if (ifs.is_open()) {
		w.write("Program");
		while(ifs.good()) {
			std::getline(ifs, line);
			sanitizeString(line);	
			processLine(parseLine(line), w);
			cout << "Processed line " << i++ << endl;
		}
		ifs.close();
	}
}

Statement* parseLine(string line) {
	if (line.find("set") == 0 || line.find("SET") == 0) {
		cout << "Picked Set to deal with " << line << endl;
		return new Set(line);
	}
	else if (line.find("jumpt") == 0 || line.find("JUMPT") == 0) {
		cout << "Picked Jumpt to deal with " << line << endl;
		return new Jumpt(line);
	}
	else if (line.find("jump") == 0 || line.find("JUMP") == 0) {
		cout << "Picked Jump to deal with " << line << endl;
		return new Jump(line);
	}
	else if (line.find("halt") == 0 || line.find("HALT") == 0) {
		cout << "Picked Halt to deal with " << line << endl;
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

void processLine(Statement *nonterm, Writer& w) {
	if (nonterm != NULL) {
		nonterm->parse(w);
		w.write("Statement");
		w.flush();
		delete nonterm;
	}
}

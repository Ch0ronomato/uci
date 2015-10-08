#include "grammer.hpp"
#include "statements.hpp"
#include "halt.hpp"
#include "writer.hpp"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

Halt::Halt(string line) : Statement(line) {
	_mvKeywords = new vector<string>();
}

vector<string>* Halt::getKeywords() {
	return _mvKeywords;
}

Grammar* Halt::parse(Writer& w) {
	if (_msLine.find(",") != string::npos) return NULL; // no comma args;
	_msLine = parseCommand(_msLine, "halt ", "HALT ");
	if (isExpr(_msLine, w)) 
		cout << "Halt========Is expression with halt" << endl;
	return this;
}

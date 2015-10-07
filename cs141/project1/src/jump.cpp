#include "grammer.hpp"
#include "statements.hpp"
#include "jump.hpp"
#include "writer.hpp"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

Jump::Jump(string line) : Statement(line) {
	_mvKeywords = new vector<string>();
}

vector<string>* Jump::getKeywords() {
	return _mvKeywords;
}

Grammar* Jump::parse(Writer& w) {
	if (_msLine.find(",") != string::npos) return NULL; // no comma args.
	_msLine = parseCommand(_msLine, "jump ", "JUMP ");
	if (isExpr(_msLine, w)) { 
		w.write("Jump");
		return this;
	}
	else return NULL;	
}

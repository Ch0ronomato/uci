#include "grammer.hpp"
#include "statements.hpp"
#include "Jumpt.hpp"
#include "writer.hpp"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

Jumpt::Jumpt(string line) : Statement(line) {
	_mvKeywords = new vector<string>();
}

vector<string>* Jumpt::getKeywords() {
	return _mvKeywords;
}

Grammar* Jumpt::parse(Writer& w) {
	if (_msLine.find(",") == string::npos) return NULL; // needs comma args.
	string left, right;
	_msLine = parseCommand(_msLine, "jumpt ", "JUMPT ");
	if (!getArguments(_msLine, &left, &right)) return NULL;
	if (isExpr(left, w)) {}

	// split right by token args (!=, ==, >, <, >=, <=)
	int pos = right.find_first_of("!=><");	
	if (right[pos + 1] == '=') {
		isExpr(right.substr(0, pos), w);
		isExpr(right.substr(pos + 3), w);
	} else {
		isExpr(right.substr(0, pos), w);
		isExpr(right.substr(pos + 2), w);
	}
	w.write("Jumpt");
	return this; 
}

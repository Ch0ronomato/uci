#include "grammer.hpp"
#include "statements.hpp"
#include "set.hpp"
#include "writer.hpp"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

Set::Set(string line) : Statement(line) {
	_mvKeywords = new std::vector<string>();
	_mvKeywords->push_back("read");
	_mvKeywords->push_back("write");
}

Set::~Set() {
	delete _mvKeywords;
}

vector<string>* Set::getKeywords() {
	return _mvKeywords;
}

Grammar* Set::parse() {
	if (_msLine.find(",") == string::npos) return NULL;
	// parse out the keyword of the command
	string left = "", right = "";
	_msLine = parseCommand(_msLine, "set ", "SET "); 
	if (!getArguments(_msLine, &left, &right)) return NULL; 
	
	// ensure both arguments are expressions;
	cout << "Set" << endl;
	if ((isExpr(left) || isKeyword(left)) && (isExpr(right) || isKeyword(right))) {
		return this;
	}
	else return NULL;
}

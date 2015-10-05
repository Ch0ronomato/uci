#include "grammer.hpp"
#include "statements.hpp"
#include "set.hpp"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

Set::Set(string line) : Statement(line) {
	_msLine = line;
}

vector<string>* Set::getKeywords() {
	cout << "Set::getKeywords" << endl;
}

Grammar* Set::parse() {
	if (_msLine.find("SET") == string::npos) return this;
	
	// parse some stuff
	_msLine
	cout << "Set::Parsing" << endl;
	return this;
}

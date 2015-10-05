#include "grammer.hpp"
#include "statements.hpp"
#include "jump.hpp"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

Jump::Jump(string line) : Statement(line) {
	_msLine = line;
}

vector<string>* Jump::getKeywords() {
	cout << "Jump::getKeywords" << endl;
}

Grammar* Jump::parse() {
	cout << "Jump::Parsing" << endl;
	return this;
}

#include "grammer.hpp"
#include "statements.hpp"
#include "halt.hpp"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

Halt::Halt(string line) : Statement(line) {
	_msLine = line;
}

vector<string>* Halt::getKeywords() {
	cout << "Halt::getKeywords" << endl;
}

Grammar* Halt::parse() {
	cout << "Halt::Parsing" << endl;
	return this;
}

#include "grammer.hpp"
#include "statements.hpp"
#include "Jumpt.hpp"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

Jumpt::Jumpt(string line) : Statement(line) {
	_msLine = line;
}

vector<string>* Jumpt::getKeywords() {
	cout << "Jumpt::getKeywords" << endl;
}

Grammar* Jumpt::parse() {
	cout << "Jumpt::Parsing" << endl;
	return this;
}

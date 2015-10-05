#include "grammer.hpp"
#include "set.hpp"
#include "statements.hpp"
#include <iostream>

using std::string;
using std::cout;
using std::endl;

int main(int argc, char ** arcv) {
	cout << "Program" << endl;
	cout << "Hello world" << endl;
	Statement *s = new Set("Hey");
	s->parse();
}

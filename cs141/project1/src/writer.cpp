#include "writer.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::ofstream;

Writer::Writer(string filename) {
	_mofsFile.open(filename.c_str());
}

void Writer::write(string line) {
	_mvLinesToWrite.push_back(line);
}

void Writer::flush() {
	for (vector<string>::reverse_iterator iter = _mvLinesToWrite.rbegin(); iter != _mvLinesToWrite.rend(); iter++)
		_mofsFile << *iter << endl;
	_mvLinesToWrite.clear();
}

Writer::~Writer() {
	_mofsFile.close();
	_mvLinesToWrite.clear();
}

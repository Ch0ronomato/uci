#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <string>
/**
 * Grammar 
 * Simple parser abstract class. Each class that inherits this
 * will be able to parse some kind of SIMPLESEM command.
 * 
 * Each grammar will hold reference to a reader class. The
 * reader will split some command by spaces, and figure out
 * what to create.
 */

class Grammar
{
public:
	virtual Grammar* parse() = 0;
protected:
	std::string parseCommand(std::string _msLine, std::string keywordLow, std::string keywordUpper) {
		if (_msLine.find(keywordUpper) == std::string::npos && _msLine.find(keywordLow) == std::string::npos) return "";
		int tokenLoc = _msLine.find(keywordUpper) ? _msLine.find(keywordUpper) : _msLine.find(keywordLow);
		return _msLine.substr(tokenLoc + keywordLow.length() + 1);
	}
};
#endif

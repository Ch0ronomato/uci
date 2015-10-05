#ifndef STATEMENT_H
#define STATEMENT_H
#include <string>
#include <vector>
#include "grammer.hpp"
class Statement : public Grammar
{
public:
	Statement(std::string line) : _msLine(line) {}
	virtual Grammar* parse() = 0;
	virtual std::vector<std::string>* getKeywords() = 0;
	bool isExpr(const std::string& s) {
		std::string ops = "+-";
		if (s.find_first_of(ops) == std::string::npos) {
			return isTerm(s);
		} else {
			std::string copy = s;
			bool good = true;
			int pos = 0;
			while ((pos = copy.find_first_of(ops) + pos) != std::string::npos && good) {
				good = isTerm(copy.substr(0, pos));
				copy = copy.erase(0, pos + 1);
			}
			return good;
		}
	};
	bool isTerm(const std::string& s) {
		std::string ops = "*/%";
		if (s.find_first_of(ops) == std::string::npos) {
			// process as normal;
			return isFactor(s);
		} else {
			std::string copy = s;
			bool good = true;
			int pos = 0, prev;
			while ((pos = copy.find_first_of(ops) + pos) != std::string::npos && good) {
				good = isFactor(copy.substr(0, pos));
				copy = copy.erase(0, pos + 1);	
			} 
			return good;
		}
	};
	bool isNumber(const std::string& s) {
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) { it++; }
		return !s.empty() && it == s.end();
	};
	bool isFactor(const std::string& s) {
		return isNumber(s) 
			|| (s[0] == 'D' && s[1] == '[' && isExpr(s.substr(2, s.find(']') - 2)))
			|| (s[0] == '(' && isExpr(s.substr(1, s.find(')') - 1)));
	};
protected:
	std::string _msLine;
};

#endif

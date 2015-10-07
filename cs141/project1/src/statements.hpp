#ifndef STATEMENT_H
#define STATEMENT_H
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include "grammer.hpp"
#include "writer.hpp"
class Statement : public Grammar
{
public:
	Statement(std::string line) : _msLine(line) { }
	virtual ~Statement() = 0;
	virtual Grammar* parse(Writer& w) = 0;
	virtual std::vector<std::string>* getKeywords() = 0;
	bool isExpr(const std::string& s, Writer& w) {
		std::string ops = "+-";
		bool good = true;
		if (s.find_first_of(ops) == std::string::npos) {
			good = isTerm(s, w);
		} else {
			std::string copy = s;
			bool good = true;
			int pos = 0;
			while ((pos = copy.find_first_of(ops) + pos) != std::string::npos && good) {
				good = isTerm(copy.substr(0, pos), w);
				copy = copy.erase(0, pos + 1);
			}
		}
		if (good) w.write("Expr");
		return good;
	};
	bool isTerm(const std::string& s, Writer& w) {
		std::string ops = "*/%";
		bool good = true;
		if (s.find_first_of(ops) == std::string::npos) {
			// process as normal;
			good = isFactor(s, w);
		} else {
			std::string copy = s;
			bool good = true;
			int pos = 0, prev;
			while ((pos = copy.find_first_of(ops) + pos) != std::string::npos && good) {
				good = isFactor(copy.substr(0, pos), w);
				copy = copy.erase(0, pos + 1);	
			} 		
		}
		if (good) w.write("Term");
		return good;
	};
	bool isNumber(const std::string& s, Writer& w) {
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it) != false) { it++; }
		bool good = !s.empty() && it == s.end();
		if (good) w.write("Number");
		return good;
	};
	bool isFactor(const std::string& s, Writer& w) {
		bool good = isNumber(s, w) 
			|| (s[0] == 'D' && s[1] == '[' && isExpr(s.substr(2, s.find(']') - 2), w))
			|| (s[0] == '(' && isExpr(s.substr(1, s.find(')') - 1), w));
		if (good) w.write("Factor");
		return good;	 
	};
	bool isKeyword(const std::string& s) {
		getKeywords();
		std::vector<std::string> *keywordsPtr = _mvKeywords;;
		if (keywordsPtr == NULL) return false;
		std::vector<std::string> keywords = *keywordsPtr;
		bool good = std::find(keywords.begin(), keywords.end(), s) != keywords.end();
		return good;
	}
	bool getArguments(std::string line, std::string *arg1, std::string *arg2) {
		std::string left = "default";
		size_t pos = line.find(",");
		if (pos == std::string::npos) return false;
		while (pos != std::string::npos) {
			*arg1 = line.substr(0, pos);
			line.erase(0, pos + 1);
			pos = line.find(",");
		} 
		if (line[0] == ' ') line.erase(0, 1);
		*arg2 = line;
		return true;
	}
protected:
	std::string _msLine;
	std::vector<std::string> *_mvKeywords;
};

inline Statement::~Statement() {}
#endif

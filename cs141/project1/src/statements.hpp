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
	virtual Grammar* parse() = 0;
	virtual std::vector<std::string>* getKeywords() = 0;
	bool isExpr(std::string s) {
		std::string output = "";
		return isExpr(s, output);
	};
	bool isExpr(std::string s, std::string& w) {
		std::string ops = "+-";
		bool good = true;
		s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
		if (s.find_first_of(ops) == std::string::npos) {
			good = isTerm(s, w);
		} else {
			good = handleOpsBubble(s, ops, true);
		}
		if (good) std::cout << "Expr: (" << s << ")\n" << w << std::endl; 
		if (s[0] == 'D' && s[1] == '[') good = good && isExpr(s.substr(2, s.find(']') - 2));
		if (s[0] == '(') good = good && isExpr(s.substr(1, s.find(')') - 1));
		return good;
	}
	bool isTerm(const std::string& s, std::string& w) {
		std::string ops = "*/%";
		bool good = true;
		if (s.find_first_of(ops) == std::string::npos) {
			// process as normal;
			good = isFactor(s, w);
		} else {
			good = handleOpsBubble(s, ops, false);
		}
		if (good) w = "Term\n" + w;
		return good;
	};
	bool isNumber(const std::string& s, std::string& w) {
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it) != false) { it++; }
		bool good = !s.empty() && it == s.end();
		if (good) w = "Number" + w;
		return good;
	};
	bool isFactor(const std::string& s, std::string& w) {
		bool good = s[0] == 'D' ? true : isNumber(s, w); 
		if (good && s[0] != 'D') w = "Factor\n" + w;
		else w = "Factor\n"; 
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
private: 
	bool handleOpsBubble(const std::string& s, std::string ops, bool op) {
		std::string out="";
		std::string copy = s + ops[0];
		bool good = true;
		size_t pos = copy.find_first_of(ops);
		while (pos != std::string::npos && good) {
			std::string temp = copy.substr(0, pos);
			good = op ? isTerm(temp, out) : isFactor(temp, out);
			copy = copy.erase(0, pos + 1);
			pos = copy.find_first_of(ops);	
			std::cout << out << std::endl;
		} 
		return good;		
	}
};

inline Statement::~Statement() {}
#endif

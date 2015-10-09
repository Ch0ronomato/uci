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
	Statement(std::string line) : _msLine(line), isInStatement(false) { }
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
		// Recursive branching.
		// If we have a line with a data read in it (e.g. D[]) then 
		// we need to make sure there is no sign of an operator. else
		// wait until the data read is parsed out
		if (s.find_first_of(ops) != std::string::npos) {
			good = handleOpsBubble(s, ops, true);
		}
		else {
			good = isTerm(s, w);
			if (good && !isInStatement) std::cout << "Expr\n" << w << std::endl;
			else if (good && isInStatement) std::cout << w << std::endl; 
			if (s[0] == 'D' && s.find_first_of(ops) == std::string::npos) {
				isInStatement = false;
				good = isExpr(s.substr(2, s.find(']') - 2));
			}
		}
		
		// output part of algorithm, and split.
		// if (s[0] == 'D' && s[1] == '[') good = good && isExpr(s.substr(2, s.find(']') - 2));
		// if (s[0] == '(') good = good && isExpr(s.substr(1, s.find(')') - 1));
		return good;
	};
	bool isTerm(const std::string& s, std::string& w) {
		std::string ops = "*/%";
		bool good = true;
		good = isFactor(s, w);
		if (good) w = "Term\n" + w;
		if (s.find_first_of(ops) != std::string::npos) {
			// process as normal;
			good = handleOpsBubble(s, ops, false);
		}
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
		else w = "Factor"; 
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
	bool isInStatement; 
	bool handleOpsBubble(const std::string& s, std::string ops, bool op) {
		std::string copy = s + ops[0];
		bool good = true;
		std::string output="";
		size_t pos = copy.find_first_of(ops);
		if (op) std::cout << "Expr" << std::endl;
		else std::cout << "Term" << std::endl;
		while (pos != std::string::npos && good) {
			std::string temp = copy.substr(0, pos);
			isInStatement = true;	
			good = temp[0] == 'D' ? isExpr(temp) : isTerm(temp, output);
			copy = copy.erase(0, pos + 1);
			pos = copy.find_first_of(ops);
			if (output != "") std::cout << output << std::endl;
			output = "";	
		} 
		return good;		
	}
};

inline Statement::~Statement() {}
#endif

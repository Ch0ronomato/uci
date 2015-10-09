#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>

using std::ifstream;
using std::string;
using std::vector;
using std::cout;
using std::endl;

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
		std::string ops = "*%/";
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

class Halt : public Statement
{
public:
	Halt(std::string);
	Grammar* parse();
	std::vector<std::string>* getKeywords();
};

Halt::Halt(string line) : Statement(line) {
	_mvKeywords = new vector<string>();
}

vector<string>* Halt::getKeywords() {
	return _mvKeywords;
}

Grammar* Halt::parse() {
	if (_msLine.find(",") != string::npos) return NULL; // no comma args;
	_msLine = parseCommand(_msLine, "halt ", "HALT ");
	isExpr(_msLine); 
	return this;
}

class Jump : public Statement
{
public:
	Jump(std::string);
	Grammar* parse();
	std::vector<std::string>* getKeywords();
};


Jump::Jump(string line) : Statement(line) {
	_mvKeywords = new vector<string>();
}

vector<string>* Jump::getKeywords() {
	return _mvKeywords;
}

Grammar* Jump::parse() {
	if (_msLine.find(",") != string::npos) return NULL; // no comma args.
	_msLine = parseCommand(_msLine, "jump ", "JUMP ");
	cout << "Jump" << endl;
	if (isExpr(_msLine)) { 
		return this;
	}
	else return NULL;	
}

class Jumpt : public Statement
{
public:
	Jumpt(std::string);
	Grammar* parse();
	std::vector<std::string>* getKeywords();
};

Jumpt::Jumpt(string line) : Statement(line) {
	_mvKeywords = new vector<string>();
}

vector<string>* Jumpt::getKeywords() {
	return _mvKeywords;
}

Grammar* Jumpt::parse() {
	if (_msLine.find(",") == string::npos) return NULL; // needs comma args.
	string left, right;
	_msLine = parseCommand(_msLine, "jumpt ", "JUMPT ");
	if (!getArguments(_msLine, &left, &right)) return NULL;
	cout << "Jumpt" << endl;
	if (isExpr(left)) {}

	// split right by token args (!=, ==, >, <, >=, <=)
	int pos = right.find_first_of("!=><");	
	if (right[pos + 1] == '=') {
		isExpr(right.substr(0, pos));
		isExpr(right.substr(pos + 3));
	} else {
		isExpr(right.substr(0, pos));
		isExpr(right.substr(pos + 2));
	}
	return this; 
}

class Set : public Statement
{
public:
	Set(std::string);
	~Set();
	Grammar* parse();
	std::vector<std::string>* getKeywords();
};


Set::Set(string line) : Statement(line) {
	_mvKeywords = new std::vector<string>();
	_mvKeywords->push_back("read");
	_mvKeywords->push_back("write");
}

Set::~Set() {
	delete _mvKeywords;
}

vector<string>* Set::getKeywords() {
	return _mvKeywords;
}

Grammar* Set::parse() {
	if (_msLine.find(",") == string::npos) return NULL;
	// parse out the keyword of the command
	string left = "", right = "";
	_msLine = parseCommand(_msLine, "set ", "SET "); 
	if (!getArguments(_msLine, &left, &right)) return NULL; 
	
	// ensure both arguments are expressions;
	cout << "Set" << endl;
	if ((isExpr(left) || isKeyword(left)) && (isExpr(right) || isKeyword(right))) {
		return this;
	}
	else return NULL;
}

Statement* parseLine(string line) {
	if (line.find("set") == 0 || line.find("SET") == 0) {
		return new Set(line);
	}
	else if (line.find("jumpt") == 0 || line.find("JUMPT") == 0) {
		return new Jumpt(line);
	}
	else if (line.find("jump") == 0 || line.find("JUMP") == 0) {
		return new Jump(line);
	}
	else if (line.find("halt") == 0 || line.find("HALT") == 0) {
		return new Halt(line);
	}
	else
		return NULL;
}

void sanitizeString(string& line) {
	if (line.find("\r") != string::npos) {
		// delete the last two characters.
		line.erase(line.find("\r"));
	}
}

void processLine(Statement *nonterm) {
	if (nonterm != NULL) {
		cout << "Statement" << endl;
		nonterm->parse();
		delete nonterm;
	}
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		cout << "Please run with filename (ex: SIMPLESEM file.S)" << endl;
		return 1;
	}
	string file(argv[argc - 1]);
	string line;
	ifstream ifs(file.c_str());

	// open file, read contents.
	int i = 0;
	if (ifs.is_open()) {
		cout << "Program" << endl;
		while(ifs.good()) {
			std::getline(ifs, line);
			sanitizeString(line);
			processLine(parseLine(line));
		}
		ifs.close();
	}
	return 0;
}


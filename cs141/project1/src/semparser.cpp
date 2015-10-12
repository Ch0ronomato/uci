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
	Statement(std::string line) : _msLine(line), isInStatement(false), isInTerm(false) { }
	virtual ~Statement() = 0;
	virtual Grammar* parse() = 0;
	virtual std::vector<std::string>* getKeywords() = 0;
	bool isExpr(std::string s) {
		std::string output = "";
		s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
		isExpr(s, output);
		std::cout << output;
		return true;
	};
	bool isExpr(std::string s, std::string& w);
	bool isTerm(const std::string& s, std::string& w);
	bool isFactor(const std::string& s, std::string& w);
	bool isNumber(const std::string& s, std::string& w) {
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it) != false) { it++; }
		bool good = !s.empty() && it == s.end();
		if (good) w += "Number\n";
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
	bool isInStatement; 
	bool isInTerm; 
	bool isInFactor;
};

inline Statement::~Statement() {}
// generic base class for recursive instantiation.
class Recurrence : public Statement
{
public: 
	Recurrence(std::string s, bool isStatement, bool isTerm, bool isFactor);
	Grammar* parse();
	std::vector<std::string>* getKeywords();
	std::string toPrint;
};

Recurrence::Recurrence(std::string s, bool isStatement, bool isTerm, bool isFactor) : Statement(s) {
	_mvKeywords = new vector<string>();
	isInStatement = isStatement;
	isInTerm = isTerm;
	isInFactor = isFactor;
}

std::vector<std::string>* Recurrence::getKeywords() {
	return _mvKeywords;
}

bool Statement::isExpr(std::string s, std::string& w) {
	// split by plus or minus, as long as they aren't nested at all.
	std::string ops = "+-";
	if (s.find_first_of(ops) == std::string::npos) {
		// no operators, yay!
		w += "Expr\n";
		isTerm(s, w);
	} else if (s.find_first_of("()[]") == std::string::npos) {
		std::string copy = s + ops[0];
		size_t pos = copy.find_first_of(ops);
		w += "Expr\n";
		Recurrence *r;
		while (pos != std::string::npos) {
			// process the invidual halfs.
			r = new Recurrence(copy.substr(0, pos), true, false, false);
			r->parse();
			w += r->toPrint;
			delete r;
			copy.erase(0, pos + 1);
			pos = copy.find_first_of(ops);	
		}
	} else {
		int nestCount = 0, lastPos = 0;
		bool any = false;
		w += "Expr\n";
		std::string copy = s;
		Recurrence *r;
		for (int i = 0; i < s.length(); i++) {
			if (s[i] == '(' || s[i] == '[') nestCount++;
			else if (s[i] == ')' || s[i] == ']') nestCount--;
			else if (nestCount == 0 && ops.find_first_of(s[i]) != std::string::npos) {
				// we found an operator, that is not nested. Cut, and process.
				std::string term = copy.substr(lastPos, i);
				r = new Recurrence(term, true, false, false);
				r->parse();
				w += r->toPrint;
				delete r;
				lastPos = i;
				any = true;
			}
		}	
		
		// call isTerm on the last piece.
		if (any) isTerm(copy.substr(lastPos + 1), w);
		else isTerm(copy, w);
	}
	return true;
};
bool Statement::isTerm(const std::string& s, std::string& w) {
	std::string ops = "*/%";
	if (s.find_first_of(ops) == std::string::npos) {
		// no operators, yay!
		w += "Term\n";
		isFactor(s, w);
	} else if (s.find_first_of("()[]") == std::string::npos) {
		// Seperate terms
		std::string copy = s + ops[0];
		size_t pos = copy.find_first_of(ops);
		Recurrence *r;
		w += "Term\n";
		while (pos != std::string::npos) {
			// process the invidual halfs.
			r = new Recurrence(copy.substr(0, pos), false, true, false);
			r->parse();
			w += r->toPrint;
			delete r;
			copy.erase(0, pos + 1);
			pos = copy.find_first_of(ops);	
		}
	} else {
		int nestCount = 0, lastPos = 0;
		bool any = false;
		std::string copy = s;
		Recurrence *r;
		w += "Term\n";
		for (int i = 0; i < s.length(); i++) {
			if (s[i] == '(' || s[i] == '[') nestCount++;
			else if (s[i] == ')' || s[i] == ']') nestCount--;
			else if (nestCount == 0 && ops.find_first_of(s[i]) != std::string::npos) {
				// we found an operator, that is not nested. Cut, and process.
				std::string term = copy.substr(lastPos, i);
				r = new Recurrence(term, false, true, false);
				r->parse();
				w += r->toPrint;
				delete r;
				lastPos = i;
				any = true;
			}
		}	
		
		// call isTerm on the last piece.
		if (any) isFactor(copy.substr(lastPos), w);
		else isFactor(copy, w);
	}
	return false;
};
bool Statement::isFactor(const std::string& s, std::string& w) {
	if (s[0] == 'D' && s[1] == '[') {
		// nested expression...yaaay.
		w += "Factor\n";
		Recurrence *r = new Recurrence(s.substr(2, s.rfind(']', s.length()) - 2), false, false, false); // allowed to expressions.
		r->parse();
		w += r->toPrint;
	} else if (s[0] == '(') {
		w+= "Factor\n";
		Recurrence *r = new Recurrence(s.substr(1, s.rfind(')', s.length()) - 1), false, false, false); // allowed to be expressions.
		r->parse();
		w += r->toPrint;
	} else {
		w += "Factor\n";
		isNumber(s, w);
	}
	return false;
};

Grammar* Recurrence::parse() {
	if (isInFactor)
		isNumber(_msLine, toPrint);
	else if (isInTerm)
		isFactor(_msLine, toPrint);
	else if (isInStatement)
		isTerm(_msLine, toPrint);
	else
		isExpr(_msLine, toPrint);
	return this;
}
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
	if ((isKeyword(left) || isExpr(left)) && (isKeyword(right) || isExpr(right))) {
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


/***************************************
 * INTERPRETER.cpp
 *
 * Student Name: Vinh Vu 
 * Student ID: 21775557 
 * UCI Net ID: vinhhv
 **************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <cstring>

#define DATA_SEG_SIZE 100

class INTERPRETER {
public:
  INTERPRETER(char *sourceFile);
  void runProgram();

private:
  std::ifstream codeIn;
  std::ifstream inputIn;
  std::ofstream outFile;

  std::vector<std::string> C;
  int D[DATA_SEG_SIZE];
  int PC;
  std::string IR;
  bool run_bit;
  char op;
  int compareOp;

  unsigned int curIRIndex;

  void printDataSeg();

  void fetch();
  void incrementPC();
  void execute();

  // Output: used in the case of: set write, source
  void write(int source);

	// Input: used in the case of: set destination, read
  int read();

  /**
   * Checks and returns if the character c is found at the current
   * position in IR. If c is found, advance to the next
   * (non-whitespace) character.
   */
  bool accept(char c);

  /**
   * Checks and returns if the string s is found at the current
   * position in IR. If s is found, advance to the next
   * (non-whitespace) character.
   */
  bool accept(const char *s);

  /**
   * Checks if the character c is found at the current position in
   * IR. Throws a syntax error if c is not found at the current
   * position.
   */
  void expect(char c);

  /**
   * Checks if the string s is found at the current position in
   * IR. Throws a syntax error if s is not found at the current
   * position.
   */
  void expect(const char *s);

  void skipWhitespace();

  void parseStatement();
  void parseSet();
  void parseJump();
  void parseJumpt();
  int parseExpr();
  int parseTerm();
  int parseFactor();
  int parseNumber();

  void syntaxError();
};


INTERPRETER::INTERPRETER(char *sourceFile) {
  codeIn.open(sourceFile, std::fstream::in);
  if (codeIn.fail()) {
    std::cerr << "init: Errors accessing source file "
              << sourceFile << std::endl;
    exit(-2);
  }

  inputIn.open("input.txt", std::fstream::in);
  if (inputIn.fail()) {
    std::cerr << "init: Errors accessing input file input.txt" << std::endl;
    exit(-2);
  }

  outFile.open((std::string(sourceFile) + ".out").c_str(), std::fstream::out);
  if (outFile.fail()) {
    std::cerr << "init: Errors accessing output file "
              << std::string(sourceFile) + ".out" << std::endl;
    exit(-2);
  }

  // Initialize the SIMPLESEM processor state
  // Initialize the Code segment
  while (codeIn.good()) {
    std::string line;
    std::getline(codeIn, line);
    C.push_back(line);
  }

  // Initialize the Data segment
  for (int i=0; i<DATA_SEG_SIZE ;i++) {
    D[i]=0;
  }
  PC = 0; // Every SIMPLESEM program begins at instruction 0
  run_bit = true; // Enable the processor
}

void INTERPRETER::runProgram() {

  while (run_bit)
  {
    fetch();
    incrementPC();
    execute();
  }

  printDataSeg();
}

void INTERPRETER::printDataSeg() {
  outFile << "Data Segment Contents" << std::endl;
  for(int i=0; i < DATA_SEG_SIZE; i++){
    outFile << i << ": " << D[i] << std::endl;
  }
}

void INTERPRETER::fetch() {
  IR = C[PC]; 
}

void INTERPRETER::incrementPC() {
  PC = PC + 1;
}

void INTERPRETER::execute() {
  curIRIndex = 0;
  parseStatement(); 
}

//Output: used in the case of: set write, source
void INTERPRETER::write(int source){
  outFile << source << std::endl;
}

//Input: used in the case of: set destination, read
int INTERPRETER::read() {
  int value;
  inputIn >> value;
  return value;
}

/**
 * Checks and returns if the character c is found at the current
 * position in IR. If c is found, advance to the next
 * (non-whitespace) character.
 */
bool INTERPRETER::accept(char c) {
  if (curIRIndex >= IR.length())
    return false;

  if (IR[curIRIndex] == c) {
    switch (IR[curIRIndex])
    {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            op = IR[curIRIndex];
            break;
        case '<':
            compareOp = 1;
            break;
        case '>':
            compareOp = 2;
            break;
    }
    curIRIndex++;
    skipWhitespace();
    return true;
  } else {
    return false;
  }
}

/**
 * Checks and returns if the string s is found at the current
 * position in IR. If s is found, advance to the next
 * (non-whitespace) character.
 */
bool INTERPRETER::accept(const char *s) {
  unsigned s_len = strlen(s);

  if (curIRIndex+s_len > IR.length())
    return false;

  for (unsigned i = 0; i < s_len; ++i) {
    if (IR[curIRIndex+i] != s[i]) {
      return false;
    }
  }
  switch (IR[curIRIndex])
  {
      case '!':
          compareOp = 3;
          break;
      case '=':
          compareOp = 4;
          break;
      case '<':
          compareOp = 5;
          break;
      case '>':
          compareOp = 6;
          break;
  }

  curIRIndex += s_len;
  skipWhitespace();
  return true;
}

/**
 * Checks if the character c is found at the current position in
 * IR. Throws a syntax error if c is not found at the current
 * position.
 */
void INTERPRETER::expect(char c) {
  if (!accept(c))
    syntaxError();
}

/**
 * Checks if the string s is found at the current position in
 * IR. Throws a syntax error if s is not found at the current
 * position.
 */
void INTERPRETER::expect(const char *s) {
  if (!accept(s))
    syntaxError();
}

void INTERPRETER::skipWhitespace() {
  while (curIRIndex < IR.length() && isspace(IR[curIRIndex]))
    curIRIndex++;
}

void INTERPRETER::parseStatement() {
  std::cerr << "Statement" << std::endl;

  if (accept("halt"))
  {
    run_bit = false;
    return;
  }
  else if(accept("set"))
    parseSet();
  else if(accept("jumpt"))
    parseJumpt();
  else if(accept("jump"))
    parseJump();
}

void INTERPRETER::parseSet() {
  std::cerr << "Set" << std::endl;
  int index;
  int value;

  bool write_bit = false;
  bool read_bit = false;

  if (!accept("write"))
    index = parseExpr();
  else
      write_bit = true;

  expect(',');

  if (!accept("read"))
    value = parseExpr();
  else
      read_bit = true;

  if (write_bit)
      write(value);
  else if (read_bit)
      D[index] = read();
  else
      D[index] = value;
}

int INTERPRETER::parseExpr() {
  std::cerr << "Expr" << std::endl;
  int number;
  number = parseTerm();

  while (accept('+') || accept('-'))
  {
    if ( op == '+' )
        number += parseTerm();
    else
        number -= parseTerm();
  }
  return number;
}

int INTERPRETER::parseTerm() {
  std::cerr << "Term" << std::endl;
  int number = parseFactor();

  while (accept('*') || accept('/') || accept('%'))
  {
    switch (op)
    {
        case '*':
            number *= parseFactor();
            break;
        case '/':
            number /= parseFactor();
            break;
        case '%':
            number %= parseFactor();
            break;
    }
  }
  return number;
}

int INTERPRETER::parseFactor() {
  std::cerr << "Factor" << std::endl;
  int number;
  if (accept("D[")) {

    number = D[parseExpr()];
    expect(']');

  } else if (accept('(')) {

    number = parseExpr();
    expect(')');

  } else {
    number = parseNumber();
  }
  return number;
}

int INTERPRETER::parseNumber() {
  std::cerr << "Number" << std::endl;
  int number = 0;
  int base = 10;
  if (curIRIndex >= IR.length())
    syntaxError();

  if (IR[curIRIndex] == '0') {
    curIRIndex++;
    skipWhitespace();
    return 0;
  } else if (isdigit(IR[curIRIndex])) {
    while (curIRIndex < IR.length() &&
           isdigit(IR[curIRIndex])) {
      number = number * base + (IR[curIRIndex] - 48);
      curIRIndex++;
    }
    skipWhitespace();
  } else {
    syntaxError();
  }
  return number;
}

void INTERPRETER::parseJump() {
  std::cerr << "Jump" << std::endl;

  PC = parseExpr();
}

//<Jumpt>-> jumpt <Expr>, <Expr> ( != | == | > | < | >= | <= ) <Expr>
void INTERPRETER::parseJumpt() {
  std::cerr << "Jumpt" << std::endl;
  int numberToJump, leftValue, rightValue;
  bool jump = false;

  numberToJump = parseExpr();

  expect(',');

  leftValue = parseExpr();

  if (accept("!=") || accept("==") || accept(">=") || accept("<=") ||
      accept('>') || accept('<')) {
    rightValue = parseExpr();
    switch (compareOp)
    {
        case 1:
            if ( leftValue < rightValue )
                jump = true;
            break;
        case 2:
            if ( leftValue > rightValue )
                jump = true;
            break;
        case 3:
            if ( leftValue != rightValue )
                jump = true;
            break;
        case 4:
            if ( leftValue == rightValue )
                jump = true;
            break;
        case 5:
            if ( leftValue <= rightValue )
                jump = true;
            break;
        case 6:
            if ( leftValue >= rightValue )
                jump = true;
            break;
    }
    if (jump)
        PC = numberToJump;
  } else {
    syntaxError();
  }
}

void INTERPRETER::syntaxError() {
  std::cerr << "Syntax Error!" << std::endl;
  exit(-1);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "init: This program requires an input "
              << "filename as a parameter " << std::endl;
    exit(-2);
  }
  INTERPRETER i(argv[1]);
  i.runProgram();
}

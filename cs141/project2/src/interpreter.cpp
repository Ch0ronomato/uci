/***************************************
 * INTERPRETER.cpp
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
  INTERPRETER(char *sourceFile, char *inputFile);
  void runProgram();
// private: uncomment in final version.
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

  void parseStatement();
  void parseSet();
  void parseJump();
  void parseJumpt();
  int parseExpr();
  int parseTerm();
  int parseFactor();
  int parseNumber();

  // optional
  void syntaxError();
};


INTERPRETER::INTERPRETER(char *sourceFile, char* inputFile) {
  codeIn.open(sourceFile, std::fstream::in);
  if (codeIn.fail()) {
    std::cerr << "init: Errors accessing source file "
              << sourceFile << std::endl;
    exit(-2);
  }

  inputIn.open(inputFile, std::fstream::in);
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
    // YOUR CODE HERE
  }

  printDataSeg();
}

void INTERPRETER::printDataSeg() {
  // DO NOT CHANGE
  outFile << "Data Segment Contents" << std::endl;
  for(int i=0; i < DATA_SEG_SIZE; i++){
    outFile << i << ": " << D[i] << std::endl;
  }
}

void INTERPRETER::fetch() {
}

void INTERPRETER::incrementPC() {
}

void INTERPRETER::execute() {
}

//DO NOT CHANGE
//Output: used in the case of: set write, source
void INTERPRETER::write(int source){
  outFile << source << std::endl;
}

//DO NOT CHANGE
//Input: used in the case of: set destination, read
int INTERPRETER::read() {
  int value;
  inputIn >> value;
  return value;
}

void INTERPRETER::parseStatement() {
  // YOUR CODE HERE
}

void INTERPRETER::parseSet() {
  // YOUR CODE HERE
}

int INTERPRETER::parseExpr() {
  // YOUR CODE HERE
}

int INTERPRETER::parseTerm() {
  // YOUR CODE HERE
}

int INTERPRETER::parseFactor() {
  // YOUR CODE HERE
}

int INTERPRETER::parseNumber() {
  // YOUR CODE HERE
}

void INTERPRETER::parseJump() {
  // YOUR CODE HERE
}

void INTERPRETER::parseJumpt() {
  // YOUR CODE HERE
}

// optional
void INTERPRETER::syntaxError() {
  // YOUR CODE HERE
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Wrong usage: ./interpreter <programfile> <inputfile>" << std::endl;
    exit(-2);
  }
  INTERPRETER i(argv[1], argv[2]);
  i.runProgram();
}

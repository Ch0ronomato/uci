#ifndef HALT_H
#define HALT_H
#include <string>
#include <vector>
#include "statements.hpp"
#include "writer.hpp"
class Halt : public Statement
{
public:
	Halt(std::string);
	Grammar* parse();
	std::vector<std::string>* getKeywords();
};
#endif


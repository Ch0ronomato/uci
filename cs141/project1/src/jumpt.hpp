#ifndef Jumpt_H
#define Jumpt_H
#include <string>
#include <vector>
#include "statements.hpp"
class Jumpt : public Statement
{
public:
	Jumpt(std::string);
	Grammar* parse();
	std::vector<std::string>* getKeywords();
};
#endif

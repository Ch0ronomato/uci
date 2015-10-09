#ifndef SET_H
#define SET_H
#include <string>
#include <vector>
#include "statements.hpp"
class Set : public Statement
{
public:
	Set(std::string);
	~Set();
	Grammar* parse();
	std::vector<std::string>* getKeywords();
};
#endif

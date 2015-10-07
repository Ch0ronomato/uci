#ifndef Jumpt_H
#define Jumpt_H
#include <string>
#include <vector>
#include "statements.hpp"
#include "writer.hpp"
class Jumpt : public Statement
{
public:
	Jumpt(std::string);
	Grammar* parse(Writer&);
	std::vector<std::string>* getKeywords();
};
#endif

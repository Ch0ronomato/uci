#ifndef Jump_H
#define Jump_H
#include <string>
#include <vector>
#include "statements.hpp"
#include "writer.hpp"
class Jump : public Statement
{
public:
	Jump(std::string);
	Grammar* parse(Writer&);
	std::vector<std::string>* getKeywords();
};
#endif

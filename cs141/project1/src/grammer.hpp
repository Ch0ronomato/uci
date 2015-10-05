#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <string>
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
};
#endif
#ifndef WRITER_H
#define WRITER_H
#include <string>
#include <vector>
#include <fstream>
class Writer 
{
public:
	Writer(std::string);
	void write(std::string);
	void flush();
	~Writer();
private:
	std::ofstream _mofsFile;
	std::vector<std::string> _mvLinesToWrite;
};
#endif

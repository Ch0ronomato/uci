#include <string>
#include "../io/iosystem.h"
#ifndef FILESYSTEM_BASE_H
#define FILESYSTEM_BASE_H
class File_system
{
public:
	virtual void create_file(std::string name) = 0;
	virtual void destroy_file(std::string name) = 0;
	virtual void open_file(std::string name) = 0;
	virtual void close_file(int oft_index) = 0;
	virtual void write_file(int index, char c, int count) = 0;
	virtual void read_file(int index, int count) = 0;
	virtual void seek_file(int index, int start) = 0;
	virtual void dir() = 0;
	virtual void save(std::string init_file) = 0;
	virtual void init(std::string init_file) = 0;
	virtual IO_system* getIO() = 0;
	virtual void setIO(IO_system*) = 0;
	static File_system *CreateFileSystem(IO_system *io);
	IO_system *io;
};
#endif
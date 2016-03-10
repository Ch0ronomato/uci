#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#ifndef LOGGER_H
#define LOGGER_H
class LoggerImpl;
class LoggerFactory {
public:
	class Ilogger
	{
	public:
		virtual void log(std::string tag, std::string message) = 0;
	};

	static Ilogger *GetLogger();
private:
	class LoggerImpl : public Ilogger {
	public:
		LoggerImpl(std::string _file) : file(std::ofstream(_file)) {

		}
		~LoggerImpl() {
			file.close();
		}
		void log(std::string tag, std::string message) {
			file << "[" << tag << std::setw(30 - tag.length()) << "]:" << message << std::endl;
		}
	private:
		std::ofstream file;
	};
	static LoggerImpl *_log;
};
#endif
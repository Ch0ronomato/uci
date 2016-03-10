#include <iostream>
#include <fstream>
#include <string>
#include "logger.h"

LoggerFactory::LoggerImpl *LoggerFactory::_log = nullptr;
LoggerFactory::Ilogger* LoggerFactory::GetLogger() {
	if (LoggerFactory::_log == nullptr) {
		LoggerFactory::_log = new LoggerFactory::LoggerImpl("log.txt");
	}
	return LoggerFactory::_log;
}	
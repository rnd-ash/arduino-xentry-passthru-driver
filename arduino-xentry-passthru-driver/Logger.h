#pragma once

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <Windows.h>

#define LOG_FILE "c:\\arduino_passthru\\activity.log"

class Logger {
public:
	void logInfo(std::string method, std::string message);
	void logWarn(std::string method, std::string message);
	void logError(std::string method, std::string message);
	std::string bytesToString(const char* bytes);
private:
	void writeToFile(std::string message);
};

extern Logger LOGGER;

#endif
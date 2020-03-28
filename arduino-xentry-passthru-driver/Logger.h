#pragma once

#include <fstream>
#include <iostream>
#include <Windows.h>

#define LOG_FILE "c:\\arduin-xentry-passthru\\sardine.log"

class Logger {
public:
	void logInfo(std::string message);
	void logWarn(std::string message);
	void logError(std::string message);
	std::string bytesToString(const char* bytes);
private:
	void writeToFile(std::string message);
};

extern Logger LOGGER;

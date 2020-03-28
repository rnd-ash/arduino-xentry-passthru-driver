#include "Logger.h"


void Logger::logInfo(std::string method, std::string message) {
	writeToFile("[INFO ] " + method + " - " + message);
}

void Logger::logWarn(std::string method, std::string message)
{
	writeToFile("[WARN ] " + method + " - " + message);
}

void Logger::logError(std::string method, std::string message)
{
	writeToFile("[ERROR] " + method + " - " + message);
}

std::string Logger::bytesToString(const char* bytes)
{
	std::string ret = "";

	char buf[4];
	for (int i = 0; i < sizeof(bytes); i++) {
		sprintf_s(buf, "%02X ", bytes[i]);
		ret += buf;
	}
	return ret;
}

void Logger::writeToFile(std::string message) {
	char time[64] = { 0x00 };
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf_s(time, "[%02d:%02d:%02d.%3d] ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	std::ofstream handle;
	try {
		handle.open(LOG_FILE, std::ios_base::app);
		handle << time << message << "\n" << std::flush;
		handle.close();
	}
	catch (std::ofstream::failure e) {
		//TODO handle error
	}
}

Logger LOGGER = Logger();
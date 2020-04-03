#pragma once

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include "j2534_v0404.h"
#include "can.h"

#define LOG_FILE "c:\\arduino_passthru\\activity.log"

class Logger {
public:
	void logInfo(std::string method, std::string message);
	void logWarn(std::string method, std::string message);
	void logError(std::string method, std::string message);
	void logInfo(std::string method, const char* fmt, ...);
	void logWarn(std::string method, const char* fmt, ...);
	void logError(std::string method, const char* fmt, ...);
	std::string payloadToString(DATA_PAYLOAD *p);
	std::string passThruMsg_toString(_PASSTHRU_MSG *msg);
	std::string frame_to_string(CAN_FRAME* f);
	std::string passThruProtocol_toString(unsigned long protocolID);
	std::string bytesToString(int size, unsigned char* bytes);
private:
	std::string argFormatToString(const char* fmt, va_list* args);
	void writeToFile(std::string message);
};

extern Logger LOGGER;

#endif
#pragma once

#ifndef ARDUINO_COMM_H_
#define ARDUINO_COMM_H_

#include <mutex>
#include <Windows.h>
#include "can.h"

namespace ArduinoComm
{
	bool OpenPort();
	void ClosePort();

	bool writeData(const char* buf, int buflen);
	bool readFrame(DATA_PAYLOAD *f);
	bool isConnected();
};

#endif


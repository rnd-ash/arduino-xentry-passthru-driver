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

	bool writeData(DATA_PAYLOAD* f);
	bool readPayload(DATA_PAYLOAD *f);
	bool isConnected();
};

#endif


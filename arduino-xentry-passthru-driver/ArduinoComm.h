#pragma once

#ifndef ARDUINO_COMM_H_
#define ARDUINO_COMM_H_

#define BAUD_RATE 115200
#define COM_PORT 4

namespace ArduinoComm
{
	enum INIT_STATE {
		OK, // Open successful
		FAIL, // Open failed
		IN_USE, // In use by something else
		ALREADY_OPEN // In use by this process
	};


	INIT_STATE OpenConnection();
	
	bool isConnected = false;

	bool SendCommand(const char* cmd);

};

#endif


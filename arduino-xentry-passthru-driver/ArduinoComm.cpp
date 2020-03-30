#include "ArduinoComm.h"
#include "pch.h"


namespace ArduinoComm {

	INIT_STATE OpenConnection() {
		return INIT_STATE::OK;
	}

	bool isConnected() {
		return true;
	}
}

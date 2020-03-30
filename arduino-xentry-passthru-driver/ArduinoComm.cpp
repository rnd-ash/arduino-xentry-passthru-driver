#include "ArduinoComm.h"
#include "pch.h"
#include "Logger.h"


namespace ArduinoComm {
	HANDLE handler;
	bool connected = false;

	std::mutex mutex;

	bool OpenPort() {
		mutex.lock();
		handler = CreateFile(L"COM4", GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (handler == INVALID_HANDLE_VALUE) {
			LOGGER.logError("ARDUINO", "Cannot create handler");
			mutex.unlock();
			return false;
		}

		DCB params = { 0x00 };
		if (!GetCommState(handler, &params)) {
			LOGGER.logError("ARDUINO", "Cannot read comm states");
			mutex.unlock();
			return false;
		}

		params.BaudRate = CBR_115200;
		params.ByteSize = 8;
		params.StopBits = ONESTOPBIT;
		params.Parity = NOPARITY;
		params.fDtrControl = DTR_CONTROL_ENABLE;

		if (!SetCommState(handler, &params)) {
			LOGGER.logError("ARDUINO", "Cannot set comm states");
			mutex.unlock();
			return false;
		}

		PurgeComm(handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
		connected = true;
		mutex.unlock();
		return true;
	}

	void ClosePort() {
		mutex.lock();
		CloseHandle(handler);
		mutex.unlock();
		connected = false;
	}

	bool writeData(const char* buf, int buflen) {
		DWORD written = 0;
		if (buflen > 255) {
			LOGGER.logError("ARDUINO", "Tx payload too big!");
			return false;
		}
		mutex.lock();
		if (!WriteFile(handler, (void*) buf, buflen, &written, NULL)) {
			LOGGER.logError("ARDUINO", "Error writing to Arduino!");
			mutex.unlock();
			return false;
		}
		char buf2[64];
		sprintf_s(buf2, "Wrote %d bytes", written);
		LOGGER.logInfo("ARDUINO", std::string(buf2));
		mutex.unlock();
		return true;
	}

	bool readFrame(DATA_PAYLOAD* f) {
		mutex.lock();
		if (!ReadFile(handler, f, sizeof(struct DATA_PAYLOAD), NULL, NULL)) {
			LOGGER.logError("ARDUINO", "Error reading from Arduino!");
			mutex.unlock();
			return false;
		}
		mutex.unlock();
		return true;
	}

	bool isConnected() {
		return connected;
	}
}

#include "ArduinoComm.h"
#include "pch.h"
#include "Logger.h"


namespace ArduinoComm {
	HANDLE handler;
	bool connected = false;

	std::mutex mutex;
	char txBuffer[255] = { 0x00 };

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
		params.fDtrControl = DTR_CONTROL_DISABLE;

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

	bool writeData(DATA_PAYLOAD* f) {
		// Result
		DWORD written = 0;
		// Too big for 1 payload for arduino - TODO - split payloads
		if (f->data_len > 245) {
			LOGGER.logError("ARDUINO", "Tx payload too big!");
			return false;
		}
		mutex.lock();
		memset(txBuffer, 0x00, sizeof(txBuffer));
		txBuffer[0] = f->type;
		txBuffer[1] = f->data_len;
		memcpy(&txBuffer[2], f->buffer, f->data_len);


		if (!WriteFile(handler, txBuffer, f->data_len + 2, &written, NULL)) {
			LOGGER.logError("ARDUINO", "Error writing to Arduino!");
			DWORD error = GetLastError();
			char buf[32];
			sprintf_s(buf, "Error code %d", error);
			LOGGER.logWarn("ARDUINO", std::string(buf));
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

#include "ArduinoComm.h"
#include "pch.h"
#include "Logger.h"


namespace ArduinoComm {
	HANDLE handler;
	bool connected = false;

	std::mutex mutex;
	char txBuffer[255] = { 0x00 };
	char rxBuffer[512] = { 0x00 };
	int pos = 0;
	bool inPayload = false;
	COMSTAT com;
	DWORD errors;
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
		txBuffer[0] = 0xBB;
		txBuffer[1] = f->data_len + 4;
		txBuffer[2] = f->type;
		txBuffer[3] = f->data_len;
		memcpy(&txBuffer[4], f->buffer, f->data_len);
		txBuffer[f->data_len + 5] = 0xAA;
		if (!WriteFile(handler, txBuffer, f->data_len + 6, &written, NULL)) {
			DWORD error = GetLastError();
			LOGGER.logWarn("ARDUINO", "Error writing data! Code %d", (int) error);
			mutex.unlock();
			return false;
		}
		char buf2[64];
		sprintf_s(buf2, "Wrote %d bytes", f->data_len+4);
		LOGGER.logInfo("ARDUINO", std::string(buf2));
		mutex.unlock();
		return true;
	}

	uint8_t len = 0xFF;
	bool readPayload(DATA_PAYLOAD* f) {
		mutex.lock();
		ClearCommError(handler, &errors, &com);
		if (com.cbInQue >= 2 && !inPayload) {
			int x = com.cbInQue;
			unsigned char read = 0x00;
			while (x > 0) {
				ReadFile(handler, &read, 1, NULL, NULL);
				if (read == 0xBB && !inPayload) {
					inPayload = true;
					ReadFile(handler, &len, 1, NULL, NULL);
					len--; // Needed as we are not counting the len byte in payload
					break;
				}
				x--;
			}
		}
		if (inPayload && com.cbInQue >= len) {
			memset(rxBuffer, 0x00, sizeof(rxBuffer));
			ReadFile(handler, rxBuffer, len, NULL, NULL);
			f->data_len = len - 2;
			f->type = rxBuffer[0];
			f->data_len = rxBuffer[1];
			memcpy(f->buffer, &rxBuffer[2], f->data_len);
			inPayload = false;
			mutex.unlock();
			return true;
		}
		mutex.unlock();
		return false;
	}

	bool isConnected() {
		return connected;
	}
}

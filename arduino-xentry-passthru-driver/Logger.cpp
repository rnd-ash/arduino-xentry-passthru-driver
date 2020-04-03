#include "Logger.h"
#include <mutex>

std::mutex mutex;
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

void Logger::logInfo(std::string method, const char* fmt, ...) {
	va_list fmtargs;
	va_start(fmtargs, fmt);
	writeToFile("[INFO ] " + method + " - " + argFormatToString(fmt, &fmtargs));
	va_end(fmtargs);
}

void Logger::logWarn(std::string method, const char* fmt, ...) {
	va_list fmtargs;
	va_start(fmtargs, fmt);
	writeToFile("[WARN ] " + method + " - " + argFormatToString(fmt, &fmtargs));
	va_end(fmtargs);
}

void Logger::logError(std::string method, const char* fmt, ...) {
	va_list fmtargs;
	va_start(fmtargs, fmt);
	writeToFile("[ERROR] " + method + " - " + argFormatToString(fmt, &fmtargs));
	va_end(fmtargs);
}

std::string Logger::payloadToString(DATA_PAYLOAD* p) {
	std::string x = "";
	char buf[4] = { 0x00 };
	for (int i = 0; i < p->data_len; i++) {
		sprintf(buf, "%02X ", p->buffer[i]);
		x += buf;
	}
	return x;
}

std::string Logger::passThruMsg_toString(_PASSTHRU_MSG *msg) {
	char buf[5120] = { 0x00 };
	if (msg != NULL) {
		sprintf_s(buf, "Protocol: %s, Payload size: %d, RxFlags: %d, TxFlags: %d, Timestamp: %d, DATA: [" ,
			passThruProtocol_toString(msg->ProtocolID).c_str(), 
			msg->DataSize,
			msg->RxStatus, 
			msg->TxFlags, 
			msg->Timestamp);
		return std::string(buf) + bytesToString(msg->DataSize, msg->Data) + "]";
	} else {
		return "NULL";
	}
}

std::string Logger::frame_to_string(CAN_FRAME *f) {
	std::string ret = "ID: ";
	char buf1[5] = { 0x00 };
	char buf2[4] = { 0x00 };
	sprintf_s(buf1, "%04X", f->can_id);
	ret += buf1;
	ret += " BYTES: ";
	for (int i = 0; i < f->can_dlc; i++) {
		sprintf_s(buf2, "%02X ", f->data[i]);
		ret += buf2;
	}
	return ret;

}

std::string Logger::passThruProtocol_toString(unsigned long protocolID) {
	switch (protocolID) {
	case J1850VPW:
		return "J1850VPW";
	case J1850PWM:
		return "J1850PWM";
	case ISO9141:
		return "ISO9141";
	case ISO14230:
		return "ISO14230";
	case CAN:
		return "CAN";
	case ISO15765:
		return "ISO15765";
	case SCI_A_ENGINE:
		return "SCI_A_ENGINE";
	case SCI_A_TRANS:
		return "SCI_A_TRANS";
	case SCI_B_ENGINE:
		return "SCI_B_ENGINE";
	case SCI_B_TRANS:
		return "SCI_B_TRANS";
	default:
		return "INVALID";
	}
}

std::string Logger::bytesToString(int size,  unsigned char* bytes)
{
	std::string ret = "";

	char buf[4] = { 0x00 };
	for (int i = 0; i < size; i++) {
		sprintf_s(buf, "%02X ", bytes[i]);
		ret += buf;
	}
	return ret;
}

std::string Logger::argFormatToString(const char* fmt, va_list* args) {
	char buffer[4096] = { 0x00 };
	int rc = vsnprintf_s(buffer, sizeof(buffer), fmt, *args);
	return std::string(buffer);
}

void Logger::writeToFile(std::string message) {
	char time[64] = { 0x00 };
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf_s(time, "[%02d:%02d:%02d.%3d] ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	std::ofstream handle;
	mutex.lock();
	try {
		handle.open(LOG_FILE, std::ios_base::app);
		handle << time << message << "\n" << std::flush;
		handle.close();
#if _DEBUG
		std::cout << message << "\n";
#endif
	}
	catch (std::ofstream::failure e) {
		//TODO handle error
	}
	mutex.unlock();
}

Logger LOGGER = Logger();
#pragma once
#include "j2534_v0404.h"
#include "can.h"

class ISO15765Hander {
public:
	bool sendPayload(PASSTHRU_MSG* msg);
	PASSTHRU_MSG recvPayload(DATA_PAYLOAD* p);
	void setMinSTTime(uint8_t minTime);
	void setBlockSize(uint8_t  blockSize);
	void setFlowID(uint32_t id);
private:
	void sendFlowControlMessage();
	uint32_t sendID = 0x0000;
	// Only for receiving payloads
	uint8_t recvBuffer[4096] = { 0x00 };
	uint8_t recvTarget = 0;
	uint8_t recvCount = 0;
	uint8_t minTime = 0;
	uint8_t blockSize = 0;
};
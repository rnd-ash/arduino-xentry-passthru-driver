#include "ISO15765Hander.h"
#include "Logger.h"
#include "ArduinoComm.h"

// Set Rx+Tx status and protocol for passthru message thats written back to Xentry
void setISOAttribtues(PASSTHRU_MSG* x) {
	x->ProtocolID = ISO15765;
	x->RxStatus = 0x00;
	x->TxFlags = 0x64;
}

void canIDIntoMsg(PASSTHRU_MSG* msg, uint32_t canID) {
	msg->Data[0] = (canID >> 24) & 0xFF;
	msg->Data[1] = (canID >> 16) & 0xFF;
	msg->Data[2] = (canID >> 8) & 0xFF;
	msg->Data[3] = canID & 0xFF;
}

bool ISO15765Hander::sendPayload(PASSTHRU_MSG* msg) {
	if (msg->DataSize - 4 < 8) {
		CAN_FRAME f = { 0x00 };
		DATA_PAYLOAD p = { 0x00 };
		f.can_id = (uint32_t)((msg->Data[0] << 24) | (msg->Data[1] << 16) | (msg->Data[2] << 8) | msg->Data[3]);
		f.can_dlc = 0x08;
		f.data[0] = msg->DataSize - 4;
		memcpy(&f.data[1], &msg->Data[4], msg->DataSize-4);
		CAN_HANDLER::frameToPayload(&p, &f);
		return ArduinoComm::writeData(&p);
	} else {
		LOGGER.logWarn("ISO15765_Send", "Not cannot send payload of %d bytes - Not implimented!", msg->DataSize - 4);
		return false;
	}
}

// Only ran once a filter has accepted the message to be a valid ISO payload
PASSTHRU_MSG ISO15765Hander::recvPayload(DATA_PAYLOAD* p) {
	PASSTHRU_MSG ret = { 0x00 };
	CAN_FRAME f = { 0x00 };
	CAN_HANDLER::payloadToFrame(p, &f);

	uint8_t pci = f.data[0] & 0xF0; // Only want the first nibble for Protocol control info (PCI)

	switch (pci) {
	case 0x00: // < 8 byte payload
		LOGGER.logInfo("ISO15765_Recv", "Single-frame Payload received with %d bytes", f.data[0]);
		ret.DataSize = f.data[0] + 4; // +4 due to header bytes
		canIDIntoMsg(&ret, f.can_id); // Insert CANID into message
		memcpy(&ret.Data[4], &f.data[1], f.data[0]); // Copy frame contents into payload
		setISOAttribtues(&ret); // Set Attributes
		break;
	case 0x10: // Start of a payload of more than 1 frame
		LOGGER.logInfo("ISO15765_Recv", "Start of multi-frame Payload. Total size is %d bytes", f.data[1]);
		// Add payload in frame to total bytes
		this->recvTarget = f.data[1]; // Register total number of bytes we should receive
		this->recvCount = 6; // Always
		memset(this->recvBuffer, 0x00, sizeof(this->recvBuffer)); // Reset buffer
		memcpy(this->recvBuffer, &f.data[2], 6); // Copy first 6 bytes to buffer
		Sleep(this->minTime / 2);
		// Now respond back to ECU
		sendFlowControlMessage();
		break;
	case 0x20: // Frame for a multi-frame payload
		LOGGER.logInfo("ISO15765_Recv", "Part of multi-frame payload received");
		memcpy(&this->recvBuffer[this->recvCount - 1], &f.data[1], 7); // Copy the next 7 bytes to buffer
		this->recvCount += 8; // Always
		if (this->recvCount >= this->recvTarget) { // Have all the bytes now!
			LOGGER.logInfo("ISO15765_Recv", "Complete payload!");
			ret.DataSize = this->recvTarget + 4; // +4 due to header bytes
			canIDIntoMsg(&ret, f.can_id); // Insert CANID into message
			memcpy(&ret.Data[4], this->recvBuffer, this->recvTarget); // Copy payload into message using the target number of bytes
			setISOAttribtues(&ret); // Set Attributes
		}
		break;
	case 0x30: // Flow control
		LOGGER.logInfo("ISO15765_Recv", "Flow control message received. Block size %d, min wait %d ms", f.data[1], f.data[2]);
		break;
	default:
		break;
	}
	return ret;
}

void ISO15765Hander::setMinSTTime(uint8_t minTime) {
	this->minTime = minTime;
}

void ISO15765Hander::setBlockSize(uint8_t blockSize) {
	this->blockSize = blockSize;
}

void ISO15765Hander::setFlowID(uint32_t id) {
	this->sendID = id;
}

void ISO15765Hander::sendFlowControlMessage() {
	LOGGER.logInfo("ISO15765_FC", "Sending flow control message. Block size %d, min wait %d ms!", this->blockSize, this->minTime);
	DATA_PAYLOAD p = { 0x00 };
	CAN_FRAME f = { 0x00 };
	f.can_id = this->sendID;
	f.can_dlc = 0x08;
	f.data[0] = 0x30;
	f.data[1] = this->blockSize;
	f.data[2] = this->minTime;
	CAN_HANDLER::frameToPayload(&p, &f);
	ArduinoComm::writeData(&p);
}

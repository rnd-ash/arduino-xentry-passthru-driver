#include "can.h"
#include "Logger.h"
#include "ArduinoComm.h"


void frameToPayload(DATA_PAYLOAD* p, CAN_FRAME* f) {
	p->type = CAN_MESSAGE_PAYLOAD;
	p->data_len = f->can_dlc + 4;
	p->buffer[0] = (f->can_id >> 24) & 0xFF;
	p->buffer[1] = (f->can_id >> 16) & 0xFF;
	p->buffer[2] = (f->can_id >> 8) & 0xFF;
	p->buffer[3] = f->can_id & 0xFF;
	memcpy(&p->buffer[4], f->data, f->can_dlc);
}

void  payloadToFrame(DATA_PAYLOAD* p, CAN_FRAME* f) {
	f->can_id = (p->buffer[0] << 24) | (p->buffer[1] << 16) | (p->buffer[2] << 8) | (p->buffer[3]);
	f->can_dlc = p->data_len - 4;
	for (int i = 0; i < f->can_dlc; i++) {
		f->data[i] = p->buffer[i + 4];
	}
}



void CAN_HANDLER::framesToPassthru(_PASSTHRU_MSG* msg, CAN_FRAME* msgs, int frames) {

}

int sepTime = 5; // TODO Seperate
bool CAN_HANDLER::sendFrame(_PASSTHRU_MSG *msg) {
	CAN_FRAME f = { 0x00 };
	if (msg->DataSize - 4 < 7) {
		f.can_id = (uint32_t) (msg->Data[0] << 24) | (msg->Data[1] << 16) | (msg->Data[2] << 8) | (msg->Data[3]);
		f.can_dlc = 8;
		f.data[0] = (uint8_t) msg->DataSize - 4;
		for (int i = 4; i < msg->DataSize; i++) {
			f.data[i - 3] = (uint8_t) msg->Data[i];
		}
		LOGGER.logInfo("CAN", "Sending ISO Frame: " + LOGGER.frame_to_string(&f));
		DATA_PAYLOAD x = { 0x00 };
		frameToPayload(&x, &f);
		return ArduinoComm::writeData(&x);
	} else {
		f.can_id = (uint32_t)(msg->Data[0] << 24) | (msg->Data[1] << 16) | (msg->Data[2] << 8) | (msg->Data[3]);
		f.can_dlc = 8;
		f.data[0] = 0x10;
		f.data[1] = (uint8_t)msg->DataSize - 4;
		for (int i = 4; i < msg->DataSize; i++) {
			f.data[i - 3] = (uint8_t)msg->Data[i];
		}
	}
	return true;
}

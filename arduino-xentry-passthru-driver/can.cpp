#include "can.h"
#include "Logger.h"
#include "ArduinoComm.h"


void CAN_HANDLER::frameToPayload(DATA_PAYLOAD* p, CAN_FRAME* f) {
	p->type = CAN_MESSAGE_PAYLOAD;
	p->data_len = f->can_dlc + 4;
	p->buffer[0] = (f->can_id >> 24) & 0xFF;
	p->buffer[1] = (f->can_id >> 16) & 0xFF;
	p->buffer[2] = (f->can_id >> 8) & 0xFF;
	p->buffer[3] = f->can_id & 0xFF;
	memcpy(&p->buffer[4], f->data, f->can_dlc);
}

void CAN_HANDLER::payloadToFrame(DATA_PAYLOAD* p, CAN_FRAME* f) {
	f->can_id = (uint32_t) (p->buffer[0] << 24) | (p->buffer[1] << 16) | (p->buffer[2] << 8) | p->buffer[3];
	f->can_dlc = p->data_len - 4;
	memcpy(f->data, &p->buffer[4], f->can_dlc);
}

#pragma once

#ifndef CAN_H_
#define CAN_H_

#include "j2534_v0404.h"
#include <stdint.h>
#include <Windows.h>
#include "can.h"

#define CAN_MESSAGE_PAYLOAD 0x01

struct CAN_FRAME {
	uint32_t can_id;
	uint8_t can_dlc;
	__declspec(align(8)) uint8_t data[8];
};

struct CAN_PAYLOAD {
	CAN_FRAME payload[10]; // Max for ISO message
	int frame_count;
};

struct DATA_PAYLOAD {
	uint8_t type;
	uint8_t data_len;
	uint8_t buffer[248];
};

namespace CAN_HANDLER {
	bool sendFrame(_PASSTHRU_MSG *msg);
	void framesToPassthru(_PASSTHRU_MSG* msg, CAN_FRAME* msgs, int frames);
};

#endif


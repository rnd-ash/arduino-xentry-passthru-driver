#pragma once

#ifndef CAN_H_
#define CAN_H_

#include "j2534_v0404.h"
#include <stdint.h>
#include <Windows.h>

#define CAN_MESSAGE_PAYLOAD 0x01;

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
	unsigned char type;
	unsigned char data_len;
	unsigned char buffer[248];
};

namespace CAN_HANDLER {
	bool sendFrame(_PASSTHRU_MSG *msg);
};

#endif


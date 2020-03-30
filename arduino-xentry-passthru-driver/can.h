#pragma once

#ifndef CAN_H_
#define CAN_H_

#include "j2534_v0404.h"
#include <stdint.h>

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned long __u32;


struct CAN_FRAME {
	__u32 can_id;
	__u8 can_dlc;
	__u8 data[8];
};

struct CAN_PAYLOAD {
	CAN_FRAME payload[10]; // Max for ISO message
	int frame_count;
};

namespace CAN_HANDLER {
	CAN_PAYLOAD sendFrame(_PASSTHRU_MSG *msg);
};

#endif


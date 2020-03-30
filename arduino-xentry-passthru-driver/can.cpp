#include "can.h"
#include "Logger.h"

CAN_PAYLOAD CAN_HANDLER::sendFrame(_PASSTHRU_MSG *msg) {
	CAN_FRAME f = { 0x00 };
	CAN_PAYLOAD pd = { 0x00 };
	if (msg->DataSize - 4 < 7) {
		f.can_id = (msg->Data[0] << 24) | (msg->Data[1] << 16) | (msg->Data[2] << 8) | (msg->Data[3]);
		f.can_dlc = 8;
		f.data[0] = (__u8) msg->DataSize - 4;
		for (int i = 4; i < msg->DataSize; i++) {
			f.data[i - 3] = msg->Data[i];
		}
		pd.payload[0] = f;
		pd.frame_count = 1;
		LOGGER.logInfo("CAN", "Sending ISO Frame: " + LOGGER.frame_to_string(&f));
		return pd;
	}
}

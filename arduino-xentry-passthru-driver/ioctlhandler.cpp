#include "ioctlhandler.h"
#include "Logger.h"
#include "j2534_v0404.h"
#include "Channel.h"


void setConfig(unsigned long ChannelID, SCONFIG_LIST* input) {
	for (int i = 0; i < input->NumOfParams; i++) {
		switch (input->ConfigPtr[i].Parameter) {
		case ISO15765_BS:
			Channel::setChannelBS(ChannelID, input->ConfigPtr[i].Value);
			break;
		case ISO15765_STMIN:
			Channel::setChannelST(ChannelID, input->ConfigPtr[i].Value);
			break;
		default:
			LOGGER.logWarn("SCONFIG", "Unknown IOCTL Set ID: %d", input->ConfigPtr[i].Parameter);
		}
	}
}

void getConfig(unsigned long ChannelID, SCONFIG_LIST* output) {
	LOGGER.logInfo("GCONFIG", "Get config Channel: %d", ChannelID);
}

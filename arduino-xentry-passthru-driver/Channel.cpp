#include "Channel.h"
#include "Logger.h"
#include "j2534_v0404.h"
#include "ArduinoComm.h"

std::map<int, Channel> Channel::channels;

Channel::Channel(unsigned long id) {
	LOGGER.logInfo("CHANNEL", "Channel %d created!", id);
	this->id = id;
	this->filter = Filter();
}

unsigned long channelCount = 0;
Channel* Channel::addChannel(unsigned long id) {
	if (channels.find(id) != channels.end()) {
		LOGGER.logInfo("CHANNEL", "Channel %d already exists!", id);
	} else {
		LOGGER.logInfo("CHANNEL", "creating channel %d", id);
		channels.insert({ id, Channel(id) });
	}
	LOGGER.logInfo("CHANNEL", "Now has %d channels!", channels.size());
	return &channels.at(id);
}

void Channel::removeChannel(unsigned long id) {
	LOGGER.logInfo("CHANNEL", "Removing channel %d", id);
	channels.erase(id);
}

Channel* Channel::getChannel(unsigned long id) {
	try {
		return &channels.at(id);
	} catch (std::exception) {
		return nullptr;
	}
}

void Channel::analyzePayload(DATA_PAYLOAD msg) {
	for (std::map<int, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
		it->second.analyzePayload(&msg);
	}
}

void Channel::setChannelBS(unsigned long id, unsigned long blockSize) {
	LOGGER.logInfo("CHANNEL", "Setting channel %d's block size to %d", id, blockSize);
	channels.at(id).setISOBlockSize(blockSize);
}

void Channel::setChannelST(unsigned long id, unsigned long min_wait_time) {
	LOGGER.logInfo("CHANNEL", "Setting channel %d's wait time to %d ms", id, min_wait_time);
	channels.at(id).setISOWaitTime(min_wait_time);
}

void Channel::setAtributes(unsigned long protocolID, unsigned long flags) {
	this->flags = flags;
	this->protocolID = protocolID;
	LOGGER.logInfo("Channel", "Setting attributes for channel %d. Protocol %d, Flags %d", (int) this->id, (int)this->protocolID, (int) this->flags);
}

void Channel::setFilters(unsigned long filterType, PASSTHRU_MSG* mask, PASSTHRU_MSG* pattern, PASSTHRU_MSG* flow) {
	filter.setType(filterType);
	filter.setMask(mask);
	filter.setPattern(pattern);
	if (protocolID == ISO15765) {
		this->handler = ISO15765Hander();
		this->handler.setFlowID((uint32_t)((flow->Data[0] << 24) | (flow->Data[1] << 16) | (flow->Data[2] << 8) | flow->Data[3]));
	}
}

void Channel::analyzePayload(DATA_PAYLOAD* msg) {
	if (this->filter.comparePayload(msg)) {
		if (this->protocolID == ISO15765) {
			PASSTHRU_MSG x = handler.recvPayload(msg);
			if (x.DataSize != 0) {
				this->queue.push(x);
			}
		} else {
			LOGGER.logWarn("CHANNEL", "Unsuppored protocol ID for incomming message %d", this->protocolID);
		}
	}
}

void Channel::setISOBlockSize(unsigned long blockSize) {
	if (this->protocolID == ISO15765) {
		handler.setBlockSize(blockSize);
	}
}

void Channel::setISOWaitTime(unsigned long minWait) {
	if (this->protocolID == ISO15765) {
		handler.setMinSTTime(minWait);
	}
}

void Filter::setType(unsigned long filterID) {
	switch (filterID) {
	case PASS_FILTER:
		this->fType = FILTER_TYPE::FILTER_TYPE_PASS;
		break;
	case BLOCK_FILTER:
		this->fType = FILTER_TYPE::FILTER_TYPE_BLOCK;
		break;
	case FLOW_CONTROL_FILTER:
		this->fType = FILTER_TYPE::FILTER_TYPE_FLOW_CTRL;
		break;
	default:
		LOGGER.logError("FILTER", "Unknown filter type %d!", (int)filterID);
	}
}

void Filter::setMask(PASSTHRU_MSG* mask) {
	LOGGER.logInfo("FILTER", "Setting mask filter. Size: %d", (int)mask->DataSize);
	memset(this->mask, 0xFF, sizeof(this->mask)); // Set whole array to 0xFF so missing data is passed
	memcpy(this->mask, mask->Data, mask->DataSize);
	this->maskSize = mask->DataSize;
}

void Filter::setPattern(PASSTHRU_MSG* pattern) {
	LOGGER.logInfo("FILTER", "Setting pattern filter. Size %d", (int) pattern->DataSize);
	memset(this->pattern, 0x00, sizeof(this->pattern));
	memcpy(this->pattern, pattern->Data, pattern->DataSize);
	this->patternSize = pattern->DataSize;
}

bool Filter::comparePayload(DATA_PAYLOAD* msg) {
	// Behaviour allow on match
	int matchedBytes = 0;
	int len = 4; // Just compare the ID - Don't actually care about the message contents (for now)
	for (int i = 0; i < len; i++) {
		// Filter pass / Flow control - Compare to pattern filter
		if (this->fType == FILTER_TYPE::FILTER_TYPE_PASS || this->fType == FILTER_TYPE::FILTER_TYPE_FLOW_CTRL) {
			if ((msg->buffer[i] & mask[i]) == pattern[i]) matchedBytes++;
		}
		// Filter type block - Compare AGAINST pattern filter
		else {
			if ((msg->buffer[i] & mask[i]) != pattern[i]) matchedBytes++;
		}
	}
	if (matchedBytes != len) {
		return false;
	}
	LOGGER.logInfo("FILTER", "Found match!: "+LOGGER.payloadToString(msg));
	return true;
}

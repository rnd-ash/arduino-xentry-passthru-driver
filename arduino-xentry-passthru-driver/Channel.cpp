#include "Channel.h"
#include "Logger.h"

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
		PASSTHRU_MSG x = { 0x00 };
		it->second.analyzePayload(&x);
	}
}

void Channel::setAtributes(unsigned long protocolID, unsigned long flags) {
	this->flags = flags;
	this->protocolID = protocolID;
	LOGGER.logInfo("Channel", "Setting attributes for channel %d. Protocol %d, Flags %d", (int) this->id, (int)this->protocolID, (int) this->flags);
}

void Channel::setFilters(unsigned long filterType, PASSTHRU_MSG* mask, PASSTHRU_MSG* pattern, PASSTHRU_MSG* flow) {
	filter.setType(filterType);
	filter.setPattern(mask);
	filter.setPattern(pattern);
	filter.setFlow(flow);
}

void Channel::analyzePayload(PASSTHRU_MSG* msg) {
	if (this->filter.analyzePayload(msg)) {
		this->queue.push(*msg);
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
	memcpy(this->mask, mask->Data, mask->DataSize);
	this->maskSize = mask->DataSize;
}

void Filter::setPattern(PASSTHRU_MSG* pattern) {
	LOGGER.logInfo("FILTER", "Setting pattern filter. Size %d", (int) pattern->DataSize);
	memcpy(this->pattern, pattern->Data, pattern->DataSize);
	this->patternSize = pattern->DataSize;
}

void Filter::setFlow(PASSTHRU_MSG* flow) {
	if (this->fType == FILTER_TYPE::FILTER_TYPE_FLOW_CTRL) {
		LOGGER.logInfo("FILTER", "Setting flow filter. Size %d", (int)flow->DataSize);
		memcpy(this->flow, flow->Data, flow->DataSize);
		this->flowSize = flow->DataSize;
	} else {
		LOGGER.logInfo("FILTER", "Not setting flow filter");
		this->flowSize = 0;
	}
}

bool Filter::analyzePayload(PASSTHRU_MSG* msg) {
	this->maskData(msg); // Masks message first
	// Now use the masked data for comparisons
	bool matches = true;
	for (int i = 0; i < patternSize; i++) {
		// Check pattern filter against the masked data
		if (pattern[i] != 0 && (pattern[i] & maskedData[i] == 0)) {
			switch (this->fType) {
			case FILTER_TYPE::FILTER_TYPE_BLOCK: // Block. So as this is 0, we allow it
				return true;
			default:
				matches = false; // Pass / flow ctrl. Doesn't match
			}
		}
	}

	// Flow control requires an extra step to verify data
	if (this->fType == FILTER_TYPE::FILTER_TYPE_FLOW_CTRL) {

	}
	
	return matches;
}

void Filter::maskData(PASSTHRU_MSG* msg) {
	memset(this->maskedData, 0x00, sizeof(this->maskedData));
	memcpy(this->maskedData, msg->Data, msg->DataSize);
	for (int i = 0; i < this->maskSize; i++) {
		this->maskedData[i] &= this->mask[i];
	}
}

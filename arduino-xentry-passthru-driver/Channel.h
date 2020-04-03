#pragma once
#include "can.h"
#include "j2534_v0404.h"
#include <map>
#include <queue>
#include <iostream>
#include "ISO15765Hander.h"

class Filter {
public:
	enum class FILTER_TYPE {
		FILTER_TYPE_PASS,
		FILTER_TYPE_BLOCK,
		FILTER_TYPE_FLOW_CTRL
	};

	void setType(unsigned long filterID);
	void setMask(PASSTHRU_MSG* mask);
	void setPattern(PASSTHRU_MSG* pattern);
	bool comparePayload(DATA_PAYLOAD* msg);
private:
	FILTER_TYPE fType;
	int maskSize = 0;
	int patternSize = 0;
	uint8_t mask[256] = { 0xFF };
	uint8_t pattern[256] = { 0x00 };
	uint8_t isoBits[256] = { 0x00 };
};

class Channel {
public:
	Channel(unsigned long id);
	static Channel* addChannel(unsigned long id);
	static void removeChannel(unsigned long id);
	static Channel* getChannel(unsigned long id);
	static void analyzePayload(DATA_PAYLOAD msg);
	static void setChannelBS(unsigned long id, unsigned long blockSize);
	static void setChannelST(unsigned long id, unsigned long min_wait_time);
	void setAtributes(unsigned long protocolID, unsigned long flags);
	void setFilters(unsigned long filterType, PASSTHRU_MSG* mask, PASSTHRU_MSG* pattern, PASSTHRU_MSG* flow);
	std::queue<PASSTHRU_MSG> queue;
private:
	ISO15765Hander handler;
	void analyzePayload(DATA_PAYLOAD* msg);
	static std::map<int,Channel> channels;
	unsigned long flags = 0;
	unsigned long id = 0;
	unsigned long protocolID = 0;
	void setISOBlockSize(unsigned long blockSize);
	void setISOWaitTime(unsigned long minWait);
	Filter filter;
};




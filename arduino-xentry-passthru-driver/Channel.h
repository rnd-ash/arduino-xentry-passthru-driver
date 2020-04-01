#pragma once
#include "can.h"
#include "j2534_v0404.h"
#include <map>
#include <queue>
#include <iostream>

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
	void setFlow(PASSTHRU_MSG* flow);
	bool analyzePayload(PASSTHRU_MSG* msg);
	void maskData(PASSTHRU_MSG* msg);
private:
	FILTER_TYPE fType;
	int maskSize = 0;
	int patternSize = 0;
	int flowSize = 0;
	char mask[4096] = { 0x00 };
	char pattern[4096] = { 0x00 };
	char flow[4096] = { 0x00 };
	char maskedData[4096] = { 0x00 };
};

class Channel {
public:
	Channel(unsigned long id);
	static Channel* addChannel(unsigned long id);
	static void removeChannel(unsigned long id);
	static Channel* getChannel(unsigned long id);
	static void analyzePayload(DATA_PAYLOAD msg);
	void setAtributes(unsigned long protocolID, unsigned long flags);
	void setFilters(unsigned long filterType, PASSTHRU_MSG* mask, PASSTHRU_MSG* pattern, PASSTHRU_MSG* flow);
	std::queue<PASSTHRU_MSG> queue;
private:
	void analyzePayload(PASSTHRU_MSG* msg);
	static std::map<int,Channel> channels;
	unsigned long flags;
	unsigned long id;
	unsigned long protocolID;
	Filter filter;
};




#pragma once
#include "j2534_v0404.h"

void setConfig(unsigned long ChannelID, SCONFIG_LIST* input);

void getConfig(unsigned long ChannelID, SCONFIG_LIST* output);


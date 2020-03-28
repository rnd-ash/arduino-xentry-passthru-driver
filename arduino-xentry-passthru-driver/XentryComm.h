#pragma once

#include "Logger.h"
#include <Windows.h>

namespace XentryComm
{
	HANDLE thread; // Comm thread
	HANDLE askInitEvent; // Handle for if other threads want to init
	HANDLE commEvent; // Handle for event from arduino
	HANDLE exitEvent; // Handle for exiting / closing thread
	HANDLE closedEvent; // Handle for when thread is closed

	bool CreateCommThread();
	void CloseCommThread();
	bool CreateEvents();
	void CloseHandles();
	DWORD WINAPI startComm(LPVOID lpParam);
};


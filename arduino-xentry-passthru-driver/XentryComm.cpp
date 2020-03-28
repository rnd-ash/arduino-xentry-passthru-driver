#include "pch.h"
#include "XentryComm.h"

namespace XentryComm{

	bool CreateCommThread() {
		// Check if thread is already running
		if (thread != NULL) {
			LOGGER.logInfo("Creating events for thread");
			if (!CreateEvents()) {
				LOGGER.logError("Failed to create events!");
				return false;
			}
			LOGGER.logInfo("Creating thread");
			thread = CreateThread(NULL, 0, startComm, NULL, 0, NULL);
			if (thread == NULL) {
				LOGGER.logError("Thread could not be created!");
				return false;
			}
			LOGGER.logInfo("Thread created!");
		}
		return true;
	}

	void CloseHandles() {
		CloseHandle(askInitEvent);
		CloseHandle(exitEvent);
		CloseHandle(commEvent);
		CloseHandle(closedEvent);
	}

	void CloseCommThread() {
		LOGGER.logInfo("Closing comm thread");
		SetEvent(exitEvent);
		WaitForSingleObject(closedEvent, 5000); // Wait for 5 seconds for the thread to terminate
		CloseHandles();
		CloseHandle(thread);
	}

	bool CreateEvents() {
		askInitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (askInitEvent == NULL) {
			LOGGER.logWarn("Cannot create init event!");
			return false;
		}

		exitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (exitEvent == NULL) {
			LOGGER.logWarn("Cannot create exit event!");
			return false;
		}
		closedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (closedEvent == NULL) {
			LOGGER.logWarn("Cannot create closed event!");
			return false;
		}
		commEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (commEvent == NULL) {
			LOGGER.logWarn("Cannot create comm event!");
			return false;
		}
		return true;
	}


	DWORD WINAPI startComm(LPVOID lpParam) {
		return 0;
	}
}
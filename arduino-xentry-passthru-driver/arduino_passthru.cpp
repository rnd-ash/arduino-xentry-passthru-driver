#include "arduino_passthru.h"
#include "Logger.h"
#include "can.h"
#include "Channel.h"
#include "ioctlhandler.h"
#include "ISO15765Hander.h"

/*
http://www.drewtech.com/support/passthru/open.html
Establish a logical communication channel with the vehicle network (via the PassThru device) using the specified network layer protocol and selected protocol options.
*/
DllExport PassThruOpen(void* pName, unsigned long* pDeviceID) {
	LOGGER.logInfo("PassThrough", "Command received: Open passthrough");
	if (pName == NULL) {
		LOGGER.logWarn("PassThrough", "pName is NULL");
	} else {
		LOGGER.logInfo("PassThrough", "pName: " + std::string((const char*) pName));
	}
	*pDeviceID = -1;
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/close.html
Close all communication with the PassThru device. All channels will be disconnected from the network, 
periodic messages will halt, and the hardware will return to its default state.
*/
DllExport PassThruClose(unsigned long DeviceID) {
	char buf[1024];
	sprintf_s(buf, "Close PassThru: DeviceID: %d", DeviceID);
	LOGGER.logInfo("PassThrough", "Command received: " + std::string(buf));
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/connect.html
Establish a logical communication channel with the vehicle network (via the PassThru device) using the specified network layer protocol and selected protocol options.
*/
DllExport PassThruConnect(unsigned long DeviceID, unsigned long ProtocolID, unsigned long Flags, unsigned long Baudrate, unsigned long* pChannelID) {
	char buf[1024];
	sprintf_s(buf, "Connect passThru: deviceID: %d, protocol: %s, Flags: %d, baudRate: %d", DeviceID, LOGGER.passThruProtocol_toString(ProtocolID).c_str(), Flags, Baudrate);
	LOGGER.logInfo("PassThrough", "Command received: " + std::string(buf));
	Channel* x = Channel::addChannel(*pChannelID);
	x->setAtributes(ProtocolID, Flags);
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/disconnect.html
Terminate an existing logical communication channel between the User Application and the vehicle network (via the PassThru device). 
Once disconnected the channel identifier or handle is invalid. For the associated network protocol this function will terminate 
the transmitting of periodic messages and the filtering of receive messages. The PassThru device periodic and filter message tables
will be cleared.
*/
DllExport PassThruDisconnect(unsigned long ChannelID) {
	char buf[1024];
	sprintf_s(buf, "Disconnect PassThru: Channel ID: %d", ChannelID);
	LOGGER.logInfo("PassThrough", "Command received: " + std::string(buf));
	Channel::removeChannel(ChannelID);
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/readmsgs.html
Receive network protocol messages, receive indications, and transmit indications from an existing logical communication channel. 
Messages will flow through PassThru device to the User Application..
*/
DllExport PassThruReadMsgs(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pNumMsgs, unsigned long Timeout) {
	Channel *x = Channel::getChannel(ChannelID);
	if (x == NULL) {
		return ERR_FAILED;
	}
	if (x->queue.size() == 0) {
		return ERR_BUFFER_EMPTY;
	}
	LOGGER.logInfo("PassThruReadMsgs", "Sending %d messages via channel %d", x->queue.size(), ChannelID);
	for (int i = 0; i < min(*pNumMsgs, x->queue.size()); i++) {
		LOGGER.logInfo("PassThrough", "Channel %d copying message to Xentry!", ChannelID);
		memcpy(&pMsg[i], &x->queue.front(), sizeof(PASSTHRU_MSG));
		x->queue.pop();
	}
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/writemsgs.html
Transmit network protocol messages over an existing logical communication channel. Messages will flow through PassThru device to the vehicle network.
*/
DllExport PassThruWriteMsgs(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pNumMsgs, unsigned long Timeout) {
	char buf[1024];
	sprintf_s(buf, "Write Msgs: ChannelID %d, TimeoutMS: %d", ChannelID, Timeout);
	LOGGER.logInfo("PassThrough", "Command received: " + std::string(buf));
	for (unsigned long i = 0; i < *pNumMsgs; i++) {
		LOGGER.logInfo("PassThrough", "--> Write Message: " + LOGGER.passThruMsg_toString(&pMsg[i]));
		if (pMsg[i].ProtocolID == ISO15765) {
			ISO15765Hander handler = ISO15765Hander();
			if (!handler.sendPayload(&pMsg[i])) {
				return ERROR_NOT_CONNECTED;
			}
		}
	}
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/startperiodicmsg.html
Repetitively transmit network protocol messages at the specified time interval over an existing logical communication channel. 
There is a limit of ten periodic messages per network layer protocol.
*/
DllExport PassThruStartPeriodicMsg(unsigned long ChannelID, PASSTHRU_MSG* pMsg, unsigned long* pMsgID, unsigned long TimeInterval) {
	char buf[1024];
	sprintf_s(buf, "Start periodic msg: Channel ID: %d, Time Interval MS: %d, MsgID: %d", ChannelID, TimeInterval, *pMsgID);
	LOGGER.logInfo("PassThrough", "Command received: " + std::string(buf));
	LOGGER.logInfo("PassThrough", "--> Periodic Message: " + LOGGER.passThruMsg_toString(pMsg));
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/stopperiodicmsg.html
Terminate the specified periodic message. Once terminated the message identifier or handle value is invalid
*/
DllExport PassThruStopPeriodicMsg(unsigned long ChannelID, unsigned long MsgID) {
	char buf[1024];
	sprintf_s(buf, "Stop periodic msg: Channel ID: %d, MsgID: %d", ChannelID, MsgID);
	LOGGER.logInfo("PassThrough", "Command received: " + std::string(buf));
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/startmsgfilter.html
The PassThruStartMsgFilter function is used to setup a network protocol filter that will selectively restrict or 
limit network protocol messages received by the PassThru device. The filter messages will flow from the User Application 
to the PassThru device. There is a limit of ten filter messages per network layer protocol.
The PassThru device will block all vehicle network receive frames by default, when no filters are defined. 
The CLEAR_RX_BUFFER (PassThruIoctl function) command must be used after establishing filters to ensure that the receive
queue only contains receive frames that adhere to the filter criteria. The PassThruStartMsgFilter function does not cause
existing receive messages to be removed from the PassThru device receive queue.
*/
DllExport PassThruStartMsgFilter(unsigned long ChannelID, unsigned long FilterType, PASSTHRU_MSG* pMaskMsg, PASSTHRU_MSG* pPatternMsg, PASSTHRU_MSG* pFlowControlMsg, unsigned long* pFilterID) {
	char buf[1024];
	sprintf_s(buf, "Start msg filter: Channel ID: %d, Filter type: %d, filterID: %d",ChannelID, FilterType, *pFilterID);
	LOGGER.logInfo("PassThrough", "Command received: " + std::string(buf));
	LOGGER.logInfo("PassThrough", "--> Mask Message: " + LOGGER.passThruMsg_toString(pMaskMsg));
	LOGGER.logInfo("PassThrough", "--> Pattern Message: " + LOGGER.passThruMsg_toString(pPatternMsg));
	LOGGER.logInfo("PassThrough", "--> Flow ctrl Message: " + LOGGER.passThruMsg_toString(pFlowControlMsg));

	Channel* x = Channel::getChannel(ChannelID);
	if (x == NULL) {
		LOGGER.logError("PassThrough", "Channel %d does not exist!", ChannelID);
		return ERR_FAILED;
	}
	x->setFilters(FilterType, pMaskMsg, pPatternMsg, pFlowControlMsg);
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/stopmsgfilter.html
Terminate the specified network protocol filter. Once terminated the filter identifier or handle value is invalid.
*/
DllExport PassThruStopMsgFilter(unsigned long ChannelID, unsigned long FilterID) {
	char buf[1024];
	sprintf_s(buf, "Stop msg filter: Channel ID: %d ,FilterID: %d", ChannelID, FilterID);
	LOGGER.logInfo("PassThrough", "Command received: " + std::string(buf));
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/setprogramming.html
Output a programmable voltage on the specified J1962 connector pin.
Only one pin can have a specified voltage applied at a time. The only exception: it is permissible to 
program pin 15 for SHORT_TO_GROUND, and another pin to a voltage level.
When switching pins, the user application must disable the first voltage (VOLTAGE_OFF option) 
before enabling the second. The user application protect against applying any incorrect voltage levels. 
A current in excess of 200mA will damage CarDAQ; do not ground the FEPS line while energized, even briefly.
*/
DllExport PassThruSetProgrammingVoltage(unsigned long DeviceID, unsigned long PinNumber, unsigned long Voltage) {
	char buf[1024];
	sprintf_s(buf, "Set programming voltage: Device ID: %d, Pin NUmber: %d, Target voltage: %d", DeviceID, PinNumber, Voltage);
	LOGGER.logInfo("PassThrough", "Command received: "+ std::string(buf));
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/readversion.html
Retrieve the PassThru device firmware version, the PassThru device DLL version, 
and the version of the J2534 specification that was referenced. The version information is in the form of NULL terminated strings.
*/
DllExport PassThruReadVersion(unsigned long DeviceID, char* pFirmwareVersion, char* pDllVersion, char* pApiVersion) {
	LOGGER.logInfo("PassThrough", "Command received:Read Version");
	// Set default values - We really don't care about the arduino software
	memcpy(pFirmwareVersion, FIRMWARE_VERSION, sizeof(FIRMWARE_VERSION));
	memcpy(pDllVersion, DLL_VERSION, sizeof(DLL_VERSION));
	memcpy(pApiVersion, API_VERSION, sizeof(API_VERSION));
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/getlasterror.html
Retrieve a text description for the most recent PassThru error as a null terminated C-string. Call this function immediately after an error occurs. 
The error string refers to the most recent function call, rather than a specific DeviceID or ChannelID, and any subsequent function call may clobber the description.
*/
DllExport PassThruGetLastError(char* pErrorDescription) {
	LOGGER.logInfo("PassThrough", "Command received: Get last error");
	return STATUS_NOERROR;
}

/*
http://www.drewtech.com/support/passthru/ioctl.html
The PassThruIoctl function is a general purpose I/O control function for modifying the vehicle network interface's characteristics.
*/
DllExport PassThruIoctl(unsigned long ChannelID, unsigned long IoctlID, void* pInput, void* pOutput) {
	SCONFIG_LIST* i = (SCONFIG_LIST*)pInput;
	SCONFIG_LIST* o = (SCONFIG_LIST*)pOutput;
	switch (IoctlID) {
	case SET_CONFIG:
		setConfig(ChannelID, i);
		break;
	case GET_CONFIG:
		getConfig(ChannelID, o);
		break;
	case READ_VBATT:
		*(unsigned long*)pOutput = (12.1 * 1000);
		break;
	default:
		LOGGER.logError("PassThruIOCTL", "Invalid IOCTL [%d]!", IoctlID);
		break;
	}
	return STATUS_NOERROR;
}

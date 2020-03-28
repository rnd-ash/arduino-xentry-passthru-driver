// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Logger.h"
#include "XentryComm.h"


bool startup() {
    LOGGER.logInfo("Setting up!");
    return XentryComm::CreateCommThread();
}

void close() {
    LOGGER.logInfo("Exiting dll");
    XentryComm::CloseCommThread();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    LOGGER.logInfo("Arduino-Xentry-Passthru v0.1");
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        LOGGER.logInfo("Process atached!");
        if (!startup()) {
            return FALSE;
        }
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        LOGGER.logInfo("Process detached!");
        close();
        break;
    }
    return TRUE;
}


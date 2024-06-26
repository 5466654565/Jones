#include "pch.h"
#include "bridge.h"

extern bool controlPanelStart;
extern bool controlPanelPause;
extern bool controlPanelStop;
extern bool controlPanelExit;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        controlPanelStart = false;
        controlPanelPause = false;
        controlPanelStop = false;
        controlPanelExit = false;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


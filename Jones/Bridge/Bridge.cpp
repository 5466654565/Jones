#include "pch.h"
#include "bridge.h"
#include <functional>

#pragma data_seg("Shared")
int currentProgressData = 0;
int currentProgressMetaData = 0;
bool controlPanelStart = false;
bool controlPanelPause = false;
bool controlPanelStop = false;
bool controlPanelExit = false;
#pragma data_seg()
#pragma comment(linker, "/section:Shared,RWS")

bool SetStart(bool boolStart) {
    controlPanelStart = boolStart;
    controlPanelPause = false;
    controlPanelStop = false;
    controlPanelExit = false;
    return controlPanelStart;
}

bool GetStart() {
    return controlPanelStart;
}

bool SetPause(bool boolPause) {
    controlPanelStart = false;
    controlPanelPause = boolPause;
    controlPanelStop = false;
    controlPanelExit = false;
    return controlPanelPause;
}

bool GetPause() {
    return controlPanelPause;
}

bool SetStop(bool boolStop) {
    controlPanelStart = false;
    controlPanelPause = false;
    controlPanelStop = boolStop;
    controlPanelExit = false;
    return controlPanelStop;
}

bool GetStop() {
    return controlPanelStop;
}

bool SetExit(bool boolExit) {
    controlPanelStart = false;
    controlPanelPause = false;
    controlPanelStop = boolExit;
    controlPanelExit = boolExit;
    return controlPanelExit;
}

bool GetExit() {
    return controlPanelExit;
}

void SetProgressData(int progressData) {
    currentProgressData = progressData;
}

int GetProgressData() {
    if (currentProgressData == 100) {
        int currentProgressData = 0;
    }
    return currentProgressData;
}

void SetProgressMetaData(int progressMetaData) {
    currentProgressMetaData = progressMetaData;
}

int GetProgressMetaData() {
    if (currentProgressMetaData == 100) {
        int currentProgressMetaData = 0;
    }
    return currentProgressMetaData;
}
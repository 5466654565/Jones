#include "BridgeCom.hpp"
#include "VectorManager.hpp"
#include <windows.h>
#include <filesystem>
#include "bridge.h"

namespace fs = std::filesystem;
bool (*BridgeCom::SetStart)(bool) = nullptr;
bool (*BridgeCom::SetPause)(bool) = nullptr;
bool (*BridgeCom::SetStop)(bool) = nullptr;
bool (*BridgeCom::SetExit)(bool) = nullptr;
bool (*BridgeCom::GetStart)() = nullptr;
bool (*BridgeCom::GetPause)() = nullptr;
bool (*BridgeCom::GetStop)() = nullptr;
bool (*BridgeCom::GetExit)() = nullptr;
void (*BridgeCom::SetProgressData)(int) = nullptr;
void (*BridgeCom::SetProgressMetaData)(int) = nullptr;

bool closingThread = false;

BridgeCom::BridgeCom() {
}

void BridgeCom::initialize() {
    HMODULE hModule = LoadLibrary(L"bridge.dll");
    if (!hModule) {
        std::cerr << "Failed to load bridge.dll" << std::endl;
        return;
    }
    SetStart = (bool (*)(bool))GetProcAddress(hModule, "SetStart");
    SetPause = (bool (*)(bool))GetProcAddress(hModule, "SetPause");
    SetStop = (bool (*)(bool))GetProcAddress(hModule, "SetStop");
    SetExit = (bool (*)(bool))GetProcAddress(hModule, "SetExit");
    GetStart = (bool (*)())GetProcAddress(hModule, "GetStart");
    GetPause = (bool (*)())GetProcAddress(hModule, "GetPause");
    GetStop = (bool (*)())GetProcAddress(hModule, "GetStop");
    GetExit = (bool (*)())GetProcAddress(hModule, "GetExit");
    SetProgressData = reinterpret_cast<void (*)(int)>(GetProcAddress(hModule, "SetProgressData"));
    SetProgressMetaData = reinterpret_cast<void (*)(int)>(GetProcAddress(hModule, "SetProgressMetaData"));

    if (!GetStart || !GetPause || !GetStop || !GetExit || !SetProgressData || !SetProgressMetaData) {
        std::cerr << "Failed to load function addresses." << std::endl;
        return;
    }

    BridgeCom::sendNoStart();

    while(!closingThread) {
		receivedStart();
		receivedPause();
		receivedStop();
        receivedExit();
	}

    FreeLibrary(hModule);

    return;
}

void BridgeCom::sendNoStart() {
	if (SetExit && SetStop && SetPause && SetStart) {
        bool ExitConfirmation =  SetExit(false);
        bool StopConfirmation = SetStop(false);
        bool PauseConfirmation = SetPause(false);
        bool StartConfirmation = SetStart(false);
	}
    return;
}

void BridgeCom::closingThreadFunction() {
    closingThread = true;
    return;
}

bool BridgeCom::receivedExit() {
    if (GetExit && GetExit()) {
        VectorManager::stopInitialization();
        BridgeCom::closingThreadFunction();
        return true;
    }
} 

void BridgeCom::receivedStop() {
    if (GetStop && GetStop()) {
        VectorManager::stopInitialization();
    }
    return;
}

void BridgeCom::receivedPause() {
    if (GetPause && GetPause()) {
        VectorManager::pauseInitialization();
    }
    return;
}

void BridgeCom::receivedStart() {
    if (GetStart && GetStart()) {
        VectorManager::startInitialization();
    }
    return;
}

bool BridgeCom::startingApp() {
    if (GetStart && GetStart()) {
        return true;
    }
}

void BridgeCom::sendDataProgress(int dataProgress) {
    if (SetProgressData) {
        SetProgressData(dataProgress);
    }
    return;
}

void BridgeCom::sendMetaDataProgress(int metaDataProgress) {
    if (SetProgressMetaData) {
        SetProgressMetaData(metaDataProgress);
    }
    return;
}

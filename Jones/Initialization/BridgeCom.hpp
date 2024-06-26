#pragma once

#include <iostream>
#include <functional>

class BridgeCom {
private :
    static bool (*SetStart)(bool);
    static bool (*SetPause)(bool);
    static bool (*SetStop)(bool);
    static bool (*SetExit)(bool);
    static bool (*GetStart)();
    static bool (*GetPause)();
    static bool (*GetStop)();
    static bool (*GetExit)();
    static void (*SetProgressData)(int);
    static void (*SetProgressMetaData)(int);

public:
    BridgeCom();
    static void initialize();
    static void closingThreadFunction();
    static void sendNoStart();
    static bool receivedExit();
    static void receivedStop();
    static void receivedPause();
    static bool startingApp();
    static void receivedStart();
    static void sendDataProgress(int value);
    static void sendMetaDataProgress(int value);

};
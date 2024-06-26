#pragma once

#include <csignal>
#include <limits>

class WindowsFunction {
public:
    static int getBatteryLevel();
    static void checkBatteryAndStopIfNeeded();
    static void monitorBattery();
    static bool isDevicePluggedIn();
};

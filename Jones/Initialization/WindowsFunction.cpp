#include "WindowsFunction.hpp"
#include "VectorManager.hpp"
#include "Config.hpp"
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>

int WindowsFunction::getBatteryLevel() {
    SYSTEM_POWER_STATUS status;
    if (GetSystemPowerStatus(&status)) {
        return status.BatteryLifePercent;
    }
    else {
        std::cerr << "Failed to get battery status" << std::endl;
    }
}

bool WindowsFunction::isDevicePluggedIn() {
    SYSTEM_POWER_STATUS status;
    if (GetSystemPowerStatus(&status)) {
        return status.ACLineStatus == 1;
    }
    else {
        std::cerr << "Failed to get AC line status" << std::endl;
        return false;
    }
}

void WindowsFunction::monitorBattery() {
    while (true) {
        int checkInterval = isDevicePluggedIn() ? 3600 : 60;
        checkBatteryAndStopIfNeeded();
        std::this_thread::sleep_for(std::chrono::seconds(checkInterval));
    }
}

void WindowsFunction::checkBatteryAndStopIfNeeded() {
    int batteryLevel = getBatteryLevel();
    int batterypercentage = Config::getBatteryThreshold();
    if (batteryLevel != -1 && batteryLevel <= batterypercentage && !isDevicePluggedIn()) {
        std::cout << "Battery is low (" << batteryLevel << "%). Stopping initialization..." << std::endl;
        VectorManager::stopInitialization();
    }
}

#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class Config {
public:
    static fs::path getInputPath();
    static fs::path getMetaDataPath();
    static fs::path getOutputPath();
    static fs::path getModelPath();
    static fs::path getArchivePath();
    static fs::path getCompletePath();
    static fs::path getErrorFilePath();
    static fs::path getStatePath();
    static const std::vector<int>& getBitGroupSizes();
    static size_t getBufferSize();
    static std::pair<double, double> getNormalizationRange();
    static double getMaxNoisePercentage();
    static int getWatchTimer();
    static int getBatteryThreshold();
};
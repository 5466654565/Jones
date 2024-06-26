#include "Config.hpp"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

fs::path Config::getModelPath() {
    return fs::path(__FILE__).parent_path().parent_path() / "model";
}

fs::path Config::getInputPath() {
    return fs::path(__FILE__).parent_path().parent_path() / "training" / "inputFile_D23";
}

fs::path Config::getMetaDataPath() {
    return fs::path(__FILE__).parent_path().parent_path() / "training" / "inputFile_D22";
}

fs::path Config::getOutputPath() {
    return fs::path(__FILE__).parent_path().parent_path() / "training" / "outputFile_D23";
}

fs::path Config::getArchivePath() {
    return fs::path(__FILE__).parent_path().parent_path() / "model" / "archive";
}

fs::path Config::getCompletePath() {
    return fs::path(__FILE__).parent_path().parent_path() / "complete";
}

fs::path Config::getErrorFilePath() {
    return fs::path(__FILE__).parent_path().parent_path() / "training" / "errorFile";
}

fs::path Config::getStatePath() {
    return fs::path(__FILE__).parent_path().parent_path() / "training" / "state";
}

const std::vector<int>& Config::getBitGroupSizes() {
    static const std::vector<int> bitGroupSizes = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 24, 32 };
    return bitGroupSizes;
}

size_t Config::getBufferSize() {
    return 5000;
}

std::pair<double, double> Config::getNormalizationRange() {
    return std::make_pair(-1.0, 1.0);
}

double Config::getMaxNoisePercentage() {
    return 0.05;  /* % */
}

int Config::getWatchTimer() {
    return 15; /* sec */
}

int Config::getBatteryThreshold() {
    return 99; /* % */
}
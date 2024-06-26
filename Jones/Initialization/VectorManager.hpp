#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <string>
#include <tuple>
#include <mutex>
#include <condition_variable>
#include <sqlite3.h>
#include <armadillo>
#include <mlpack/core.hpp>
#include "VectorKey.hpp"
#include "Config.hpp"
#include "FileManager.hpp"

typedef std::tuple<int, int, int, std::string, int> VectorKey;  // (Matter, Content, Bit, Int, VectorIndex)

class VectorManager {
private:
    size_t hashVector(const std::vector<double>& vect);
    sqlite3* db;
    std::string modelPath;
    long long initializationLine;
    std::string storagePath;
    std::unordered_map<VectorKey, std::vector<double>, VectorKeyHash, VectorKeyEqual> bitVectors;
    std::unordered_set<size_t> vectorHashes;
    std::map<std::tuple<int, int, int, std::string>, int> lastIndex;
    const size_t bufferSize = Config::getBufferSize();
    static std::vector<std::string> outputBuffer;
    int groupSizeLoopIndex;
    int rowLoopIndex;
    int bitLoopIndex;
    arma::mat data;
    int totalLines ;
    int lastProgress;
    std::string state;
    double position;
    double cumulativeFrequency;
    double entropy;
    double normPosition;
    double normCumulativeFrequency;
    double normEntropy;
    double totalPosition;
    double totalCumulativeFrequency;
    double totalEntropy;
    int totalGroups;
    double minVal;
    double maxVal;
    double minPosition;
    double maxPosition;
    double minCumulativeFrequency;
    double maxCumulativeFrequency;
    double minEntropy;
    double maxEntropy;
    double avgNormPosition;
    double avgNormCumulativeFrequency;
    double avgNormEntropy;

    // Supporting functionalities
    void exportToMLPack(const std::string& csvFilePath, const std::string& txtFilePath);
    void statsBitGroupSizes(int groupSizeIndex);
    void buildVectorBuffer(const VectorKey& key, const std::vector<double>& vect, const std::string& state);
    bool saveVectorData();
    void loadVectors();
    void flushVectorBuffer();
    void getSingeltonValue();
    void getConfigValue();
    void pauseCheck();
    void resetVectorStats();
    void resetVector();
    void resetProgress();
    bool checkDuplicate(const std::vector<double>& vect);
    void vectorQuantity(const std::string& csvFilePath);
    double noiseGenerator();

public:
    static bool initializationData;
    static bool initializationMetaData;
    static std::string lastMessage;
    static std::atomic<bool> paused;
    static std::atomic<bool> stoped;
    static std::mutex mtx;
    
    // Core functionalities
    bool initialization(const std::string& csvFilePath, const std::string& txtFilePath);
    VectorManager();
    ~VectorManager();

    // Serialization and deserialization
    std::string keyToText(const VectorKey& key);
    std::string vectorToText(const std::vector<double>& vect);
    VectorKey textToKey(const std::string& text);
    std::vector<double> textToVector(const std::string& text);

    // Control Panel functionalities
    static void pauseInitialization();
    static void stopInitialization();
    static void startInitialization();
    static void setInitializationData(bool value);
    static void setInitializationMetaData(bool value);

};
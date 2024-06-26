#include "VectorManager.hpp"
#include "FileManager.hpp"
#include "MathematicalFonction.hpp"
#include "Singleton.hpp"
#include "BridgeCom.hpp""
#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>
#include <bitset>
#include <numeric>
#include <random>
#include <chrono>
#include <tuple>

std::vector<std::string> VectorManager::outputBuffer;
bool VectorManager::initializationData = false;
bool VectorManager::initializationMetaData = false;
std::string VectorManager::lastMessage = "";
int groupSizeLoopIndex = 0;
int rowLoopIndex = 0;
int bitLoopIndex = 0;
arma::mat data;
int totalLines = 0;
int lastProgress = 0;
std::string state = "";
double position = 0.0;
double cumulativeFrequency = 0.0;
double entropy = 0.0;
double normPosition = 0.0;
double normCumulativeFrequency = 0.0;
double normEntropy = 0.0;
double totalPosition = 0.0;
double totalCumulativeFrequency = 0.0;
double totalEntropy = 0.0;
int totalGroups = 0;
double minVal = 0.0;
double maxVal = 0.0;
double minPosition = 0.0;
double maxPosition = 0.0;
double minCumulativeFrequency = 0.0;
double maxCumulativeFrequency = 0.0;
double minEntropy = 0.0;
double maxEntropy = 0.0;
double avgNormPosition = 0.0;
double avgNormCumulativeFrequency = 0.0;
double avgNormEntropy = 0.0;
std::atomic<bool> VectorManager::paused(false);
std::atomic<bool> VectorManager::stoped(false);
std::mutex VectorManager::mtx;

VectorManager::VectorManager() {

    modelPath = Config::getModelPath().string();

    std::filesystem::path dbDirectory = std::filesystem::path(modelPath);
    if (!std::filesystem::exists(dbDirectory)) {
        if (!std::filesystem::create_directories(dbDirectory)) {
            std::cerr << "Failed to create directory: " << dbDirectory << std::endl;
            return;
        }

        std::string testFilePath = modelPath + "\\testfile.txt";
        std::ofstream testFile(testFilePath);
        if (!testFile) {
            std::cerr << "Failed to create test file at: " << testFilePath << std::endl;
            return;
        }
        testFile.close();
        std::filesystem::remove(testFilePath);

        loadVectors();
    }
    return;
}

void VectorManager::loadVectors() {

    sqlite3* db;
    sqlite3_stmt* stmt;
    std::string dbPath = modelPath + "\\model.db";

    std::ofstream dbTestFile(dbPath, std::ios::app);
    if (!dbTestFile) {
        std::cerr << "Cannot open or create the database file at: " << dbPath << std::endl;
        return;
    }
    dbTestFile.close();

    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "loadVectors fonction : Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    const char* sql = "SELECT D1, D2, D3, Bit, Key, X0, Y0, Z0, X1, Y1, Z1 FROM jonesTable;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int D1 = sqlite3_column_int(stmt, 0);
        int D2 = sqlite3_column_int(stmt, 1);
        int D3 = sqlite3_column_int(stmt, 2);
        const char* Bit = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        int Key = sqlite3_column_int(stmt, 4);
        std::vector<double> vect;
        for (int i = 5; i < 11; i++) {
            vect.push_back(sqlite3_column_double(stmt, i));
        }

        VectorKey vectorKey = std::make_tuple(D1, D2, D3, std::string(Bit), Key);
        bitVectors[vectorKey] = vect;


        std::tuple<int, int, int, std::string> subKey = make_tuple(D1, D2, D3, std::string(Bit));
        if (lastIndex.find(subKey) == lastIndex.end() || lastIndex[subKey] < Key) {
            lastIndex[subKey] = Key + 1;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

VectorManager::~VectorManager() {
    if (!outputBuffer.empty()) {
        VectorManager::flushVectorBuffer();
    }
}

std::string VectorManager::keyToText(const VectorKey& key) {
    std::ostringstream ss;
    ss << std::get<0>(key) << ','
        << std::get<1>(key) << ','
        << std::get<2>(key) << ','
        << std::get<3>(key) << ','
        << std::get<4>(key);
    return ss.str();
}

std::string VectorManager::vectorToText(const std::vector<double>& vect) {
    std::ostringstream ss;
    for (auto& val : vect) {
        if (&val != &vect[0]) {
            ss << ',';
        }
        ss << val;
    }
    return ss.str();
}

VectorKey VectorManager::textToKey(const std::string& text) {
    std::istringstream ss(text);
    std::string segment;
    std::vector<int> seglist;

    while (std::getline(ss, segment, ',')) {
        seglist.push_back(std::stoi(segment));
    }

    return std::make_tuple(seglist[0], seglist[1], seglist[2], std::to_string(seglist[3]), seglist[4]);
}

std::vector<double> VectorManager::textToVector(const std::string& text) {
    std::vector<double> vect;
    std::istringstream ss(text);
    std::string segment;

    while (std::getline(ss, segment, ',')) {
        vect.push_back(std::stod(segment));
    }

    return vect;
}

size_t VectorManager::hashVector(const std::vector<double>& vect) {
    std::hash<double> hasher;
    size_t hash = 0;
    for (double val : vect) {
        hash ^= hasher(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

void VectorManager::vectorQuantity(const std::string& csvFilePath) {
    VectorManager::resetProgress();
    std::ifstream file(csvFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << csvFilePath << std::endl;
        FileManager::moveFileToSibling(csvFilePath, "errorFile");
        return;
    }
    arma::mat data;
    if (!mlpack::data::Load(csvFilePath, data, true)) {
        FileManager::moveFileToSibling(csvFilePath, "errorFile");
        std::cerr << "Failed to load CSV data" << std::endl;
        return;
    }
    int totalVectors = 0;
    auto bitGroupSizes = Config::getBitGroupSizes();
    for (int groupSize : bitGroupSizes) {
        for (int row = 0; row < data.n_rows; ++row) {
            for (int bitIndex = 0; bitIndex < data.n_cols; bitIndex += groupSize) {
                std::vector<int> groupBits;
                for (int i = 0; i < groupSize; ++i) {
                    if (bitIndex + i < data.n_cols) {
                        groupBits.push_back(data(row, bitIndex + i));
                    }
                }
                if (groupBits.size() == groupSize) {
                    totalVectors++;
                }
            }
        }
    }
    totalLines = totalVectors;
    initializationLine = totalVectors;
}

bool VectorManager::checkDuplicate(const std::vector<double>& vect) {
    size_t vectHash = hashVector(vect);
    if (vectorHashes.find(vectHash) != vectorHashes.end()) {
        return true;
    }
    else {
        vectorHashes.insert(vectHash);
        return false;
    }
}

void VectorManager::flushVectorBuffer() {

    if (saveVectorData()) {
        FileManager::rotationStateInsertInData();
        outputBuffer.clear();
    }
}

bool VectorManager::saveVectorData() {  

    sqlite3* db;
    char* errMsg = nullptr;
    std::string dbPath = modelPath + "\\model.db";

    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "saveData() : Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errMsg);

    for (const std::string& line : outputBuffer) {
        std::istringstream iss(line);
        std::vector<std::string> parts;
        std::string part;
        while (std::getline(iss, part, ',')) {
            parts.push_back(part);
        }

        VectorKey key = std::make_tuple(std::stoi(parts[0]), std::stoi(parts[1]), std::stoi(parts[2]), parts[3], std::stoi(parts[4]));
        std::vector<double> vect;
        for (size_t i = 5; i < parts.size(); ++i) {
            vect.push_back(std::stod(parts[i]));
        }

        sqlite3_stmt* stmt;
        const char* sql = "INSERT INTO jonesTable (D1, D2, D3, Bit, Key, X0, Y0, Z0, X1, Y1, Z1) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {

            sqlite3_bind_int(stmt, 1, std::get<0>(key));
            sqlite3_bind_int(stmt, 2, std::get<1>(key));
            sqlite3_bind_int(stmt, 3, std::get<2>(key));
            sqlite3_bind_text(stmt, 4, std::get<3>(key).c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 5, std::get<4>(key));
            for (int i = 0; i < 6; ++i) {
                sqlite3_bind_double(stmt, 6 + i, vect[i]);
            }
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
            }
            sqlite3_finalize(stmt);
        }
        else {
            std::cerr << "Preparation Error: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    if (errMsg == nullptr) {
        sqlite3_exec(db, "COMMIT;", NULL, NULL, &errMsg);
    }
    else {
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, &errMsg);
    }

    sqlite3_close(db);
    return errMsg == nullptr;   
}

void VectorManager::buildVectorBuffer(const VectorKey& key, const std::vector<double>& vect, const std::string& state) {
    
    try {
        std::string keyLine = VectorManager::keyToText(key);
        std::string vectorLine = VectorManager::vectorToText(vect);
        FileManager::rotationStateInConstruction(keyLine, vectorLine, state);
        }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
	}
    
    bitVectors[key] = vect;

    std::ostringstream ss;
    ss << std::get<0>(key) << ',' << std::get<1>(key) << ',' << std::get<2>(key) << ',' << std::get<3>(key) << ',' << std::get<4>(key);
    for (double v : vect) {
        ss << ',' << v;
    }
    ss << ',' << state;

    outputBuffer.push_back(ss.str());
    if (outputBuffer.size() >= bufferSize) {
        VectorManager::flushVectorBuffer();;
    }

    return;
}

void VectorManager::getSingeltonValue() {
	Singleton& singleton = *(Singleton::getInstance());
    groupSizeLoopIndex = singleton.getStartGroupSize();
    rowLoopIndex = singleton.getStartRow();
    bitLoopIndex = singleton.getStartBitIndex();
	return;
}

void VectorManager::getConfigValue() {
    auto normalizationRange = Config::getNormalizationRange();
    minVal = normalizationRange.first;
    maxVal = normalizationRange.second;
    return;
}

void VectorManager::exportToMLPack(const std::string& csvFilePath, const std::string& txtFilePath) {
    if (!mlpack::data::Load(csvFilePath, data, true, false)) {
        FileManager::moveFileToSibling(csvFilePath, "errorFile");
        FileManager::moveFileToSibling(txtFilePath, "errorFile");
        std::cerr << "Failed to load CSV data" << std::endl;
    }
    return;
}

void VectorManager::resetProgress() {
    totalLines = 0;
    lastProgress = 0;
	return;
}

void VectorManager::resetVectorStats() {
    totalPosition = 0.0;
    totalCumulativeFrequency = 0.0;
    totalEntropy = 0.0;
    totalGroups = 0;
    
    minVal = 0.0;
    maxVal = 0.0;
    minPosition = 0.0;
    maxPosition = 0.0;
    minCumulativeFrequency = 0.0;
    maxCumulativeFrequency = 0.0;
    minEntropy = 0.0;
    maxEntropy = 0.0;

    avgNormPosition = 0.0;
    avgNormCumulativeFrequency = 0.0;
    avgNormEntropy = 0.0;
    return;
}

void VectorManager::resetVector() {
    position = 0.0;
    cumulativeFrequency = 0.0;
    entropy = 0.0;
}

void VectorManager::pauseCheck() {
    if (paused) {
        VectorManager::flushVectorBuffer();
        while (paused) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

double VectorManager::noiseGenerator() {
    std::pair<double, double> rangeValue = Config::getNormalizationRange();
    std::default_random_engine generator(std::random_device{}());
    std::uniform_real_distribution<double> distribution(rangeValue.first, rangeValue.second);
    double randomValue = distribution(generator);
    double maxNoisePercentage = Config::getMaxNoisePercentage();
    double noise = 1 + (maxNoisePercentage * randomValue);

    return noise;
}

void VectorManager::statsBitGroupSizes(int groupSizeIndex) {

    VectorManager::resetVector();

    auto bitGroupSizes = Config::getBitGroupSizes();

    if (groupSizeIndex >= bitGroupSizes.size()) {
        std::cerr << "Error: Invalid groupSizeIndex" << std::endl;
        return;
    }
    int groupSize = bitGroupSizes[groupSizeIndex];
    
    int globalBitIndex = 0;
    int rowIndex = 0;
    int bitIndex = 0;

    minPosition = std::numeric_limits<double>::max();
    maxPosition = std::numeric_limits<double>::lowest();
    minCumulativeFrequency = std::numeric_limits<double>::max();
    maxCumulativeFrequency = std::numeric_limits<double>::lowest();
    minEntropy = std::numeric_limits<double>::max();
    maxEntropy = std::numeric_limits<double>::lowest();
    
    while (globalBitIndex < data.n_elem) {
        std::vector<int> groupBits;
        int bitsCollected = 0;

        while (bitsCollected < groupSize && globalBitIndex < data.n_elem) {
            rowIndex = globalBitIndex / data.n_cols;
            bitIndex = globalBitIndex % data.n_cols;

            groupBits.push_back(data(rowIndex, bitIndex));
            bitsCollected++;
            globalBitIndex++;
        }

        if (groupBits.size() == groupSize) {
            std::string bitGroupAsString = "";
            for (int bit : groupBits) {
                bitGroupAsString += std::to_string(bit);
            }
                            
            VectorManager::getConfigValue();
            double noise = VectorManager::noiseGenerator();
            position = static_cast<double>(bitIndex) / data.n_cols * noise;
            cumulativeFrequency = static_cast<double>(std::accumulate(groupBits.begin(), groupBits.end(), 0)) / groupBits.size() * noise;
            entropy = MathematicalFonction::calculateEntropy(groupBits) * noise;

            totalPosition += position;
            totalCumulativeFrequency += cumulativeFrequency;
            totalEntropy += entropy;
            totalGroups++;

            minPosition = std::min(minPosition, position);
            maxPosition = std::max(maxPosition, position);
            minCumulativeFrequency = std::min(minCumulativeFrequency, cumulativeFrequency);
            maxCumulativeFrequency = std::max(maxCumulativeFrequency, cumulativeFrequency);
            minEntropy = std::min(minEntropy, entropy);
            maxEntropy = std::max(maxEntropy, entropy);

            avgNormPosition = (totalPosition / totalGroups - minPosition) / (maxPosition - minPosition) * (maxVal - minVal) + minVal;
            avgNormCumulativeFrequency = (totalCumulativeFrequency / totalGroups - minCumulativeFrequency) / (maxCumulativeFrequency - minCumulativeFrequency) * (maxVal - minVal) + minVal;
            avgNormEntropy = (totalEntropy / totalGroups - minEntropy) / (maxEntropy - minEntropy) * (maxVal - minVal) + minVal;
        }
    }
    VectorManager::resetVector();
    return;
}

bool VectorManager::initialization(const std::string& csvFilePath, const std::string& txtFilePath) {
    
    VectorManager::getSingeltonValue();
    VectorManager::exportToMLPack(csvFilePath, txtFilePath);
    VectorManager::vectorQuantity(csvFilePath);

    auto bitGroupSizes = Config::getBitGroupSizes();
    for (int groupSizeIndex = groupSizeLoopIndex; groupSizeIndex < bitGroupSizes.size(); ++groupSizeIndex) {
        int groupSize = bitGroupSizes[groupSizeIndex];

        VectorManager::pauseCheck();
        if (stoped) {
            VectorManager::flushVectorBuffer();
            FileManager::saveState(csvFilePath, state);
            return false;
        }
        VectorManager::statsBitGroupSizes(groupSizeIndex);

        int globalBitIndex = 0;
        int rowIndex = 0;
        int bitIndex = 0;

        while (globalBitIndex < data.n_elem) {
            std::vector<int> groupBits;
            int bitsCollected = 0;

            while (bitsCollected < groupSize && globalBitIndex < data.n_elem) {
                rowIndex = globalBitIndex / data.n_cols;
                bitIndex = globalBitIndex % data.n_cols;

                groupBits.push_back(data(rowIndex, bitIndex));
                bitsCollected++;
                globalBitIndex++;
            }

            if (groupBits.size() == groupSize) {
                std::string bitGroupAsString = "";
                for (int bit : groupBits) {
                    bitGroupAsString += std::to_string(bit);
                }

                double noise = VectorManager::noiseGenerator();
                position = static_cast<double>(bitIndex) / data.n_cols * noise;
                cumulativeFrequency = static_cast<double>(std::accumulate(groupBits.begin(), groupBits.end(), 0)) / groupBits.size() * noise;
                entropy = MathematicalFonction::calculateEntropy(groupBits) * noise;

                normPosition = (position - minPosition) / (maxPosition - minPosition) * (maxVal - minVal) + minVal;
                normCumulativeFrequency = (cumulativeFrequency - minCumulativeFrequency) / (maxCumulativeFrequency - minCumulativeFrequency) * (maxVal - minVal) + minVal;
                normEntropy = (entropy - minEntropy) / (maxEntropy - minEntropy) * (maxVal - minVal) + minVal;

                std::tuple<int, int, int, std::string> subKey = make_tuple(0, 3, groupSize, bitGroupAsString);
                int key = lastIndex[subKey];
                lastIndex[subKey]++;

                VectorKey vectorKey = std::make_tuple(0, 3, groupSize, bitGroupAsString, key);
                std::vector<double> vec = {
                    avgNormPosition,
                    avgNormCumulativeFrequency,
                    avgNormEntropy,
                    normPosition,
                    normCumulativeFrequency,
                    normEntropy
                };

                std::string state = std::to_string(groupSizeIndex) + "," + std::to_string(0) + "," + std::to_string(0);
                    
                VectorManager::buildVectorBuffer(vectorKey, vec, state);
                VectorManager::resetVector();

                initializationLine--;
                int progress = (1 - static_cast<float>(initializationLine) / totalLines) * 100;
                if (progress - lastProgress >= 1) {
                    if (VectorManager::initializationData) {
                        BridgeCom::sendDataProgress(progress);
                    }
                    if (VectorManager::initializationMetaData) {
                        BridgeCom::sendMetaDataProgress(progress);
                    }
                    lastProgress = progress;
                }
            }

        }
        VectorManager::resetVectorStats();
    }
    if (!outputBuffer.empty()) {
        VectorManager::flushVectorBuffer();
    }
    groupSizeLoopIndex = 0;

    Singleton::getInstance()->setStartGroupSize(int(0));

    return true;
}

void VectorManager::pauseInitialization() {
    std::lock_guard<std::mutex> lock(mtx);
    paused = true;
    if (lastMessage != "INITIALIZATION PAUSED") {
        std::cout << "INITIALIZATION PAUSED" << std::endl;
        lastMessage = "INITIALIZATION PAUSED";
    }
    return;
}

void VectorManager::stopInitialization() {
    std::lock_guard<std::mutex> lock(mtx);
    stoped = true;
    if (lastMessage != "INITIALIZATION STOPED") {
        std::cout << "INITIALIZATION STOPED" << std::endl;
        lastMessage = "INITIALIZATION STOPED";
    }
    return;
}

void VectorManager::startInitialization() {
    std::lock_guard<std::mutex> lock(mtx);
    paused = false;
    stoped = false;
    if (lastMessage != "INITIALIZATION RUNNING") {
        std::cout << "INITIALIZATION RUNNING" << std::endl;
        lastMessage = "INITIALIZATION RUNNING";
    }
    return;
}

void VectorManager::setInitializationData(bool value) {
    initializationData = value;
    return;
}

void VectorManager::setInitializationMetaData(bool value) {
    initializationMetaData = value;
    return;
}
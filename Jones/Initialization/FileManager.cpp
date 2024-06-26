#include "FileManager.hpp"
#include <bitset>
#include <iostream>
#include <filesystem>
#include <fstream> 
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <ctime>
#include <string>
#include <algorithm>
#include <vector>

bool FileManager::fonctionCheckPoint = false;
namespace fs = std::filesystem;
std::string FileManager::errorFilePath = Singleton::getInstance()->getSingletonErrorFilePath();
int FileManager::startGroupSize = 0;
int FileManager::startRow = 0;
int FileManager::startBitIndex = 0;
bool FileManager::loadStateMetaData = false;
std::string FileManager::reloadFilePath;

std::string FileManager::convertToBinary(const std::string& inputFilePath) {

    std::string newCSV = FileManager::createCSV(inputFilePath);

    std::string outputFilePath = FileManager::moveFileToSibling(newCSV, outputPath);

    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open inputFile for conversion." << std::endl;
        FileManager::moveFileToSibling(inputFilePath, "errorFile");
        return "";
    }

    if (!outputFile.is_open()) {
        std::cerr << "Failed to open outputFile for conversion." << std::endl;
        FileManager::moveFileToSibling(outputFilePath, "errorFile");
        return "";
    }

    char byteGroup;
    while (inputFile.read(&byteGroup, 1)) {
        std::bitset<8> bits(byteGroup);
        for (int i = 7; i >= 0; --i) {
            outputFile << bits[i] << (i > 0 ? "," : "\n");
        }
    }

    inputFile.close();
    outputFile.close();

    return outputFilePath;
}

std::string FileManager::moveFileToSibling(const std::string& filePath, const std::string& subFolder) {
    if (fonctionCheckPoint) {
        std::cerr << "Fonction Checkpoint : moveFileToSibling" << std::endl;
    }
    std::string fileName = fs::path(filePath).filename().string();
    std::string dirPath = (fs::path(filePath).parent_path().parent_path() / subFolder).string();
    if (!fs::exists(dirPath)) {
        fs::create_directory(dirPath);
    }
    std::string newPath = dirPath + "\\" + fileName;
    fs::rename(filePath, newPath);
    return newPath;
}

std::string FileManager::moveFileToSiblingError(const std::string& filePath) {
    if (fonctionCheckPoint) {
        std::cerr << "Fonction Checkpoint : moveFileToSibling" << std::endl;
    }
    std::string fileName = fs::path(filePath).filename().string();
    std::filesystem::path dirPath = fs::path(errorFilePath);
    if (!fs::exists(dirPath)) {
        fs::create_directory(dirPath);
    }
    std::filesystem::path newPath = dirPath / fileName;
    fs::rename(filePath, newPath);
    return newPath.string();;
}

std::string FileManager::createCSV(const std::string& inputFilePath) {
    if (FileManager::fonctionCheckPoint) {
        std::cerr << "Fonction Checkpoint : createCSV" << std::endl;
    }

    fs::path inputPath(inputFilePath);

    fs::path outputFilePath = inputPath;
    outputFilePath.replace_extension(".csv");

    std::ofstream outputFile(outputFilePath);
    outputFile.close();

    return outputFilePath.string();
}

std::string FileManager::createTxt(const std::string& inputFilePath) {
    if (FileManager::fonctionCheckPoint) {
        std::cerr << "Fonction Checkpoint : createCSV" << std::endl;
    }

    fs::path inputPath(inputFilePath);

    fs::path outputFilePath = inputPath;
    outputFilePath.replace_extension(".txt");

    std::ofstream outputFile(outputFilePath);
    outputFile.close();

    return outputFilePath.string();
}

std::string FileManager::addDateNTime(const std::string& filePath) {
    if (FileManager::fonctionCheckPoint) {
        std::cerr << "Fonction Checkpoint : addDateNTime" << std::endl;
    }

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm buf;
    localtime_s(&buf, &in_time_t);
    std::ostringstream timeStream;
    timeStream << std::put_time(&buf, "_%Y%m%d_%H%M");
    std::string timestamp = timeStream.str();

    fs::path pathObj(filePath);
    std::string newFileName = pathObj.stem().string() + timestamp + pathObj.extension().string();
    fs::path newFilePath = pathObj.parent_path() / newFileName;

    try {
        fs::rename(pathObj, newFilePath);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to rename file: " << e.what() << std::endl;
        return "";
    }
    return newFilePath.string();
}

std::string FileManager::initialWatcher() {
    std::vector<fs::path> files;

    if (!reloadFilePath.empty()) {
        if (!loadStateMetaData) {
            return reloadFilePath;
        }
    }

    for (const auto& entry : fs::directory_iterator(inputPath)) {
        if (entry.path().extension() == ".txt") {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end(), [](const fs::path& a, const fs::path& b) {
        return fs::last_write_time(a) < fs::last_write_time(b);
        });

    if (!files.empty()) {
        for (const auto& file : files) {
            if (fs::file_size(file) > 0) {
                return addDateNTime(file.string());
            } else {
                FileManager::moveFileToSiblingError(file.string());
			}
        }
    }

    return "";
}

std::string FileManager::metaDataWatcher() {
    std::vector<fs::path> files;

    if (!reloadFilePath.empty() && loadStateMetaData) {
        return reloadFilePath;
    }

    for (const auto& entry : fs::directory_iterator(metaDataPath)) {
        if (entry.path().extension() == ".txt") {
            files.push_back(entry.path());
        }
    }

    if (files.empty()) {
        return "";
    }

    std::sort(files.begin(), files.end(), [](const fs::path& a, const fs::path& b) {
        return fs::last_write_time(a) < fs::last_write_time(b);
        });

    if (!files.empty()) {
        for (const auto& file : files) {
            if (fs::file_size(file) > 0) {
                continue;
            }
            else {
                FileManager::moveFileToSiblingError(file.string());
            }
        }
    }

    fs::path oldestFile = files.front();

    if (oldestFile.filename().string().find("_metadata.txt") != std::string::npos) {
        return oldestFile.string();
    }
    else {
        return addDateNTime(oldestFile.string());
    }
}


std::string FileManager::createMetadata(const std::string& filePath) {

    try {
        fs::path fullPath(filePath);
        fs::path directoryPath = fullPath.parent_path();
        std::string textToFind = "_metadata.txt";
        std::string metaDataFilePath = FileManager::getFilePath(textToFind, directoryPath.string());
        if (std::ifstream stateFile(metaDataFilePath); stateFile.is_open()) {
            stateFile.close();
            return metaDataFilePath;
        }
    }
    catch (const std::exception& e) {}

    fs::path originalPath(filePath);
    fs::path metadataPath = originalPath.parent_path() / (originalPath.stem().string() + "_metadata.txt");
    std::ofstream metaFile(metadataPath);
    if (!metaFile.is_open()) {
        std::cerr << "Failed to create metadata file." << std::endl;
        return "";
    }

    std::string file_name = fs::path(filePath).filename().string();

    std::string absolute_path = fs::absolute(filePath).string();

    std::uintmax_t file_size = fs::file_size(filePath);

    auto ftime = fs::last_write_time(filePath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
    std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

    std::ifstream file(filePath);
    std::string line;
    int line_count = 0;
    while (std::getline(file, line)) {
        ++line_count;
    }

    file.close();

    fs::perms p = fs::status(filePath).permissions();

    std::tm tm;
    localtime_s(&tm, &cftime);
    char timeStr[26];
    asctime_s(timeStr, sizeof(timeStr), &tm);

    metaFile << "File Name: " << file_name << std::endl;
    metaFile << "Absolute Path: " << absolute_path << std::endl;
    metaFile << "File Size: " << file_size << " bytes" << std::endl;
    metaFile << "Last Modification Date: " << timeStr;
    metaFile << "Line Count: " << line_count << std::endl;
    metaFile << "Permissions: "
        << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
        << ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
        << ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
        << std::endl;

    metaFile.close();

    std::string movedPath = FileManager::moveFileToSibling(filePath, "complete");
    if (movedPath.empty()) {
        std::cerr << "Failed to move file to complete: " << filePath << std::endl;
    }

    return metadataPath.string();
}

void FileManager::saveState(const std::string& inputFilePath, const std::string& state) {
    fs::path originalPath(inputFilePath);
    fs::path modifiedPath = originalPath.parent_path() / (originalPath.stem().string() + "_state.txt");

    if (!fs::exists(originalPath.parent_path())) {
        std::cerr << "Parent directory does not exist: " << originalPath.parent_path() << std::endl;
        if (!fs::create_directories(originalPath.parent_path())) {
            std::cerr << "Failed to create parent directory." << std::endl;
            return;
        }
    }

    std::ofstream outputFile(modifiedPath);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open outputFile for state saving." << std::endl;
        return;
    }

    outputFile << state;
    outputFile.close();

    if (!fs::exists(modifiedPath)) {
        std::cerr << "Failed to create state file: " << modifiedPath << std::endl;
    }
    else {
        if (!fs::remove(inputFilePath)) {
            std::cerr << "Failed to delete the state file: " << inputFilePath << std::endl;
        }
    }
}

std::string FileManager::getFilePath(const std::string& textToFind, const std::string& inputString) {
    fs::path inputPath(inputString);
    std::string outputFilePath;

    for (const auto& entry : fs::directory_iterator(inputPath)) {
        if (entry.path().filename().string().ends_with(textToFind)) {
            outputFilePath = entry.path().string();
            std::ifstream stateFile(outputFilePath);
            if (stateFile.is_open()) {
                stateFile.close();
                return outputFilePath;
            }
        }
    }
    return "";
}


void FileManager::loadState() {
    std::string outputPath = Singleton::getInstance()->getSingletonOutputPath();
    std::string inputPath = Singleton::getInstance()->getSingletonInputPath();
    std::string metaDataPath = Singleton::getInstance()->getSingletonMetaDataPath();

    for (const auto& entry : fs::directory_iterator(outputPath)) {
        if (entry.path().filename().string().ends_with("_metadata_state.txt")) {
            loadStateMetaData = true;
        }
    }

    std::string textToFind = "_state.txt";

    std::string stateFilePath = FileManager::getFilePath(textToFind, outputPath);
    std::ifstream stateFile(stateFilePath);
    if (!stateFile.is_open()) {
        return;
    }

    std::string line;
    if (std::getline(stateFile, line)) {
        std::istringstream ss(line);

        std::string token;

        std::getline(ss, token, ',');
        startGroupSize = std::stoi(token);

        std::getline(ss, token, ',');
        startRow = std::stoi(token);

        std::getline(ss, token, ',');
        startBitIndex = std::stoi(token);
    }

    Singleton::getInstance()->setStartGroupSize(FileManager::startGroupSize);
    Singleton::getInstance()->setStartRow(FileManager::startRow);
    Singleton::getInstance()->setStartBitIndex(FileManager::startBitIndex);

    fs::path filePath(stateFilePath);
    std::string baseName = filePath.stem().string();
    baseName.erase(baseName.find("_state"), 6);

    if (loadStateMetaData) {
        reloadFilePath = metaDataPath + "/" + baseName + ".txt";
    }
    else {
        reloadFilePath = inputPath + "/" + baseName + ".txt";
    }

    std::ifstream txtFile(reloadFilePath);
    if (!txtFile.is_open()) {
        std::cerr << "Failed to reload file: " << reloadFilePath << std::endl;
        FileManager::startGroupSize = 0;
        FileManager::startRow = 0;
        FileManager::startBitIndex = 0;
        FileManager::reloadFilePath = "";
        FileManager::moveFileToSiblingError(stateFilePath);
        return;
    }

    stateFile.close();
    txtFile.close();

    if (!fs::remove(stateFilePath)) {
        std::cerr << "Failed to delete the state file: " << stateFilePath << std::endl;
    }

    if (stateFile.is_open()) {
        std::cerr << "Failed to delete the state file: " << stateFilePath << std::endl;
        stateFile.close();
    }
    return;
}

void FileManager::rotationStateInConstruction(std::string& keyLine, std::string& vectorLine, const std::string& state) {
    try {
        fs::path statePath = Singleton::getInstance()->getSingletonStatePath();
        fs::path stateFilePath = statePath / "inConstruction.txt";

        std::ofstream inConstructionFile(stateFilePath, std::ios::app);

        if (!inConstructionFile.is_open()) {
            inConstructionFile.open(stateFilePath, std::ios::out | std::ios::app);
            if (inConstructionFile.is_open()) {
            }
            else {
                std::cerr << "Failed to create file inConstruction.txt at: " << stateFilePath << std::endl;
            }
        }

        if (inConstructionFile.is_open()) {
            inConstructionFile << keyLine << ',' << vectorLine << ',' << state << std::endl;
            inConstructionFile.close();
        }
        else {
            std::cerr << "Unable to open file inConstruction for writing: " << stateFilePath << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to write to file: " << e.what() << std::endl;
    }
    return;
}


void FileManager::rotationStateInsertInData() {
    try {
        fs::path statePath = Singleton::getInstance()->getSingletonStatePath();
        fs::path currentFilePath = statePath / "inConstruction.txt";
        fs::path insertInDataPath = statePath / "insertInData.txt";

        if (fs::exists(insertInDataPath)) {
            fs::remove(insertInDataPath);
        }

        if (fs::exists(currentFilePath)) {
            fs::rename(currentFilePath, insertInDataPath);
        }
        else {
            std::cerr << "Unable to open file insertInData for writing: " << insertInDataPath << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to write to file: " << e.what() << std::endl;
    }
    return;
}


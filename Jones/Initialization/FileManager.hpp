#pragma once

#include <string>
#include <filesystem>
#include <ctime>
#include "Singleton.hpp" 

class FileManager {

private:
    std::string inputPath;
    std::string metaDataPath;
    std::string outputPath;
    std::string modelPath;
    std::string archivePath;
    std::string completePath;
    static std::string errorFilePath;
    std::vector<std::string> processedFiles;

public:
    static int startGroupSize;
    static int startRow;
    static int startBitIndex;
    static bool loadStateMetaData;
	static std::string reloadFilePath;
    static std::string outputFilePath;
    
    static std::string moveFileToSibling(const std::string& filePath, const std::string& subFolder);
    static std::string moveFileToSiblingError(const std::string& filePath);
    static std::string getFilePath(const std::string& textToFind, const std::string& inputString);
    static std::string createCSV(const std::string& inputFilePath);
    static std::string createTxt(const std::string& inputFilePath);
    static std::string addDateNTime(const std::string& filePath);
    std::string convertToBinary(const std::string& inputFilePath);
    std::string initialWatcher();
    std::string metaDataWatcher();
    static std::string createMetadata(const std::string& filePath);
    static void saveState(const std::string& inputFilePath, const std::string& state);
    static void loadState();
    FileManager() : 
        inputPath(Singleton::getInstance()->getSingletonInputPath()),
        metaDataPath(Singleton::getInstance()->getSingletonMetaDataPath()),
        outputPath(Singleton::getInstance()->getSingletonOutputPath()),
        modelPath(Singleton::getInstance()->getSingletonModelPath()),
        archivePath(Singleton::getInstance()->getSingletonArchivePath()),
        completePath(Singleton::getInstance()->getSingletonCompletePath()) {}

    static bool fonctionCheckPoint; 
    static void rotationStateInConstruction(std::string& keyLine, std::string& vectorLine, const std::string& state);
    static void rotationStateInsertInData();
};
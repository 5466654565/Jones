#include "Singleton.hpp"
#include "Config.hpp"

Singleton* Singleton::instance = nullptr;


Singleton::Singleton() {
    Config config;
    inputPath = config.getInputPath().string();
    metaDataPath = config.getMetaDataPath().string();
    outputPath = config.getOutputPath().string();
    modelPath = config.getModelPath().string();
    archivePath = config.getArchivePath().string();
    completePath = config.getCompletePath().string();
    errorFilePath = config.getErrorFilePath().string();
    statePath = config.getStatePath().string();
    singletonStartGroupSize = 0;
    singletonStartBitIndex = 0;
    singletonStartRow = 0;
}

Singleton* Singleton::getInstance() {
    if (!instance)
        instance = new Singleton;
    return instance;
}

std::string Singleton::getSingletonInputPath() {
    return inputPath;
}

std::string Singleton::getSingletonMetaDataPath() {
    return metaDataPath;
}

std::string Singleton::getSingletonOutputPath() {
    return outputPath;
}

std::string Singleton::getSingletonModelPath() {
    return modelPath;
}

std::string Singleton::getSingletonArchivePath() {
    return archivePath;
}

std::string Singleton::getSingletonCompletePath() {
    return completePath;
}

std::string Singleton::getSingletonErrorFilePath() {
    return errorFilePath;
}

std::string Singleton::getSingletonStatePath() {
    return statePath;
}

void Singleton::setStartGroupSize(int startGroupSize) {
    singletonStartGroupSize = startGroupSize;
    return;
}

int Singleton::getStartGroupSize() const {
    return singletonStartGroupSize;
}

void Singleton::setStartRow(int startRow) {
    singletonStartRow = startRow;
    return;
}

int Singleton::getStartRow() const {
    return singletonStartRow;
}

void Singleton::setStartBitIndex(int startBitIndex) {
    singletonStartBitIndex = startBitIndex;
    return;
}

int Singleton::getStartBitIndex() const {
    return singletonStartBitIndex;
}


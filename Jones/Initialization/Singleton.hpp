#pragma once

#include <string>
#include <mutex>
#include <condition_variable> 
#include <thread>

class Singleton {
private:
    static Singleton* instance;
    Singleton();
    std::mutex mtx;
    std::condition_variable cv;
    std::thread timerThread;
    std::string inputPath;
    std::string metaDataPath;
    std::string outputPath;
    std::string modelPath;
    std::string archivePath;
    std::string completePath;
    std::string errorFilePath;
    std::string statePath;
    int singletonStartGroupSize;
    int singletonStartBitIndex;
    int singletonStartRow;

public:
    static Singleton* getInstance();
    std::string getSingletonInputPath();
    std::string getSingletonMetaDataPath();
    std::string getSingletonOutputPath();
    std::string getSingletonModelPath();
    std::string getSingletonArchivePath();
    std::string getSingletonCompletePath();
    std::string getSingletonErrorFilePath();
    std::string getSingletonStatePath();
    void setStartGroupSize(int startGroupSize);
    int getStartGroupSize() const;
    void setStartBitIndex(int startBitIndex);
    int getStartBitIndex() const;
    void setStartRow(int startRow);
    int getStartRow() const;
};

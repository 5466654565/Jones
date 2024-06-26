#include <iostream>
#include <mlpack/core.hpp>
#include "ExceptionManager.hpp"
#include "VectorKey.hpp"
#include "Config.hpp"
#include "MathematicalFonction.hpp"
#include "HashData.hpp"
#include "FileManager.hpp"
#include "VectorManager.hpp"
#include "Singleton.hpp"
#include "BridgeCom.hpp"
#include "WindowsFunction.hpp""
#include "bridge.h"

using namespace std::chrono;
namespace fs = std::filesystem;

std::condition_variable cv;
std::mutex mtx;
bool readyToWatch = false;
bool continueTimer = true;
bool shutdown = false;
int watchTimer = Config::getWatchTimer();
std::string lastMainMessage = "";
std::string lastMainMessage2 = "";
bool startApp = false;

void timerFunction() {
    while (continueTimer) {
        std::this_thread::sleep_for(seconds(watchTimer));
        {
            std::lock_guard<std::mutex> lock(mtx);
            readyToWatch = true;
        }
        cv.notify_one();
        
        if(shutdown) {
            continueTimer = false;
            break;
		}
    }

    return;
}

std::thread timerThread(timerFunction);

void bridgeFunction() {
    BridgeCom::initialize();
}

std::thread bridgeThread(bridgeFunction);

void powerManagement() {
    while (!shutdown) {
        
        std::this_thread::sleep_for(std::chrono::seconds(5));

        if (!WindowsFunction::isDevicePluggedIn()) {
            WindowsFunction::checkBatteryAndStopIfNeeded();
            if (lastMainMessage != "Lost power, on battery. Stopping at 50%.") {
                std::cout << "Lost power, on battery. Stopping at 50%." << std::endl;
                lastMainMessage = "Lost power, on battery. Stopping at 50%.";
            }
        }
        
        if (BridgeCom::receivedExit()) {
            shutdown = true;
            break;
        }
    }

    return;
}

std::thread powerThread(powerManagement);

int main() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    while (1 == 1) {
        while (!startApp) {
            if(BridgeCom::startingApp()) {
                bool startApp = true;
                break;
			}
            std::this_thread::sleep_for(std::chrono::seconds(1));
	    }

        if (shutdown) {
            break;
        }

        std::cout << "Application initialization..." << std::endl;
        fs::path currentPath = fs::current_path();
        ExceptionManager::setupExceptionHandling();
        FileManager fileManager;
        VectorManager vectorManager;

        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] { return readyToWatch; });

            if (shutdown) {
                break;
            }

            while (VectorManager::stoped) {
                if (shutdown) {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            FileManager::loadState();

            while (readyToWatch) {
                if (lastMainMessage2 != "Starting application...") {
                    std::cout << "Starting application..." << std::endl;
                    lastMainMessage2 = "Starting application...";
                }
                if (shutdown) {
                    break;
                }
                try {
                    if (!FileManager::loadStateMetaData) {
                        /* Step : File detected */
                        std::string inputFilePath = fileManager.initialWatcher();
                        if (!inputFilePath.empty()) {
                            /* Step : Convert Data into Binary */
                            std::cout << "Starting Data initialization" << std::endl;
                            std::string csvFilePath = fileManager.convertToBinary(inputFilePath);
                            VectorManager::setInitializationData(true);
                            if (csvFilePath.empty()) {
                                FileManager::moveFileToSiblingError(inputFilePath);
                            }
                            else {

                                /* Step : Initialization Data */
                                if (vectorManager.initialization(csvFilePath, inputFilePath) == false) {
                                    break;
                                }
                                else {

                                    /* Step : Create MetaData*/
                                    FileManager::moveFileToSibling(csvFilePath, "complete");
                                    FileManager::moveFileToSibling(inputFilePath, "inputFile_D22");
                                    int completeDataProgress = 0;
                                    BridgeCom::sendDataProgress(completeDataProgress);
                                    FileManager::reloadFilePath = "";
                                    VectorManager::setInitializationData(false);
                                    std::cout << "Data initialized." << std::endl;
                                }
                            }
                        }
                    }

                    if (shutdown) {
                        break;
                    }

                    /* Step : File for MetaData detected */
                    std::string inputMetaDataPath = fileManager.metaDataWatcher();
                    if (!inputMetaDataPath.empty()) {

                        /* Step : Create MetaData */
                        std::cout << "Starting MetaData initialization" << std::endl;
                        std::string outputMetaDataPath = fileManager.createMetadata(inputMetaDataPath);
                        if (outputMetaDataPath.empty()) {
                            FileManager::moveFileToSiblingError(inputMetaDataPath);
                        }
                        else {

                            /* Step : Convert MetaData into Binary */
                            std::string csvMetaDataPath = fileManager.convertToBinary(outputMetaDataPath);
                            VectorManager::setInitializationMetaData(true);
                            if (csvMetaDataPath.empty()) {
                                FileManager::moveFileToSiblingError(outputMetaDataPath);
                            }
                            else {

                                /* Step : Initialization MetaData */
                                if (vectorManager.initialization(csvMetaDataPath, outputMetaDataPath) == false) {
                                    break;
                                }
                                else {
                                    FileManager::moveFileToSibling(outputMetaDataPath, "complete");
                                    FileManager::moveFileToSibling(csvMetaDataPath, "complete");
                                    int completeMetaDataProgress = 0;
                                    BridgeCom::sendMetaDataProgress(completeMetaDataProgress);
                                    VectorManager::setInitializationMetaData(false);
                                    std::cout << "MetaData initialized." << std::endl;
                                    FileManager::loadStateMetaData = false;
                                }
                            }
                        }
                    }
                    FileManager::reloadFilePath = "";
                    readyToWatch = false;

                    Singleton::getInstance()->setStartGroupSize(int(0));
                    Singleton::getInstance()->setStartRow(int(0));
                    Singleton::getInstance()->setStartBitIndex(int(0));

                    if (shutdown) {
                        break;
                    }
                }
                catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Filesystem error: " << e.what() << std::endl;
                    std::cerr << "Error code: " << e.code() << " (" << e.code().message() << ")" << std::endl;
                }
                catch (const std::exception& e) {
                    std::cerr << "Erreur : " << e.what() << std::endl;
                }
                catch (...) {
                    std::cerr << "Erreur inconnue capturée dans le bloc principal." << std::endl;
                }
            }
        }

    }

    try {
        std::cout << "Shutingdown application..." << std::endl;
        if (timerThread.joinable())
            timerThread.join();
        if (powerThread.joinable())
            powerThread.join();
        if (bridgeThread.joinable())
            bridgeThread.join();
        std::cout << "...Safely Exiting" << std::endl;
        }
    catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Erreur inconnue capturée dans le bloc principal." << std::endl;
	}

    exit(0);
}
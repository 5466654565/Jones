#include "ExceptionManager.hpp"
#include <iostream>

void ExceptionManager::handleException() {
    try {
        throw;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception non gérée interceptée: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Exception non gérée de type inconnu capturée" << std::endl;
    }
    std::cout << "Appuyez sur Entrée pour quitter..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    abort();
}

void ExceptionManager::handleAbort(int signum) {
    std::cerr << "Signal d'arrêt reçu (SIGABRT)" << std::endl;
    handleException();
}

void ExceptionManager::setupExceptionHandling() {
    set_terminate(handleException);
    signal(SIGABRT, handleAbort);
}
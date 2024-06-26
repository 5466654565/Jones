#pragma once

#include <iostream>
#include <exception>
#include <csignal>
#include <limits>

class ExceptionManager {
public:
    static void handleException();
    static void handleAbort(int signum);
    static void setupExceptionHandling();
};

#include "StdLogger.h"

#include <iostream>

namespace arv {

    void StdLogger::LogInfoMessage(const std::string &message) {
        std::cout << "[INFO] " << message << std::endl;
    }

    void StdLogger::LogWarnMessage(const std::string &message) {
        std::cout << "[WARN] " << message << std::endl;
    }

    void StdLogger::LogErrorMessage(const std::string &message) {
        std::cout << "[ERROR] " << message << std::endl;
    }

}
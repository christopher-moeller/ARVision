#pragma once

#include "utils/Logger.h"

namespace arv {

    class StdLogger : public arv::Logger {

    protected:
        void LogInfoMessage(const std::string &message) override;
        void LogWarnMessage(const std::string &message) override;
        void LogErrorMessage(const std::string &message) override;

    };

}
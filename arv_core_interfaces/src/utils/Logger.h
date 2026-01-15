#pragma once

#include <string>
#include <sstream>
#include <cstdarg>
#include <vector>

namespace arv {
    
    class Logger {
      
    public:
        template<typename... Args>
        void Info(const std::string& format, Args&&... args) {
            std::string message = formatString(format, std::forward<Args>(args)...);
            LogInfoMessage(message);
        }
        
        template<typename... Args>
        void Error(const std::string& format, Args&&... args) {
            std::string message = formatString(format, std::forward<Args>(args)...);
            LogErrorMessage(message);
        }
        
    protected:
        virtual void LogInfoMessage(const std::string& message) = 0;
        virtual void LogErrorMessage(const std::string& message) = 0;
        
    private:
        template<typename... Args>
        static std::string formatString(const std::string& format, Args&&... args) {
            std::ostringstream oss;
            size_t argIndex = 0;
            std::vector<std::string> values = {toString(std::forward<Args>(args))...};

            for (size_t i = 0; i < format.length(); ++i) {
                if (format[i] == '{' && i + 1 < format.length() && format[i + 1] == '}') {
                    if (argIndex < values.size()) {
                        oss << values[argIndex++];
                        i++;  // Skip the '}'
                    } else {
                        oss << "{}";  // Placeholder without matching argument
                    }
                } else {
                    oss << format[i];
                }
            }

            return oss.str();
        }
      
        // Utility to convert any type to a string
        template<typename T>
        static std::string toString(T&& value) {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
    };

}
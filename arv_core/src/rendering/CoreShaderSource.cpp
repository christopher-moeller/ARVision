#include <sstream>
#include <algorithm>

#include "CoreShaderSource.h"

namespace arv {

    // Helper function to check if string starts with prefix (C++17 compatible)
    static bool startsWith(const std::string& str, const std::string& prefix)
    {
        return str.size() >= prefix.size() &&
               str.compare(0, prefix.size(), prefix) == 0;
    }

    // Helper function to check if string ends with suffix (C++17 compatible)
    static bool endsWith(const std::string& str, const std::string& suffix)
    {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    // Helper function to trim leading whitespace from a string
    static std::string trimLeadingWhitespace(const std::string& str)
    {
        size_t start = str.find_first_not_of(" \t");
        if (start == std::string::npos) return "";
        return str.substr(start);
    }

    // Helper function to trim leading and trailing blank lines
    static std::string trimBlankLines(const std::string& str)
    {
        if (str.empty()) return str;

        // Find first non-whitespace line
        size_t start = 0;
        size_t end = str.size();

        // Trim leading blank lines
        size_t pos = 0;
        while (pos < str.size())
        {
            size_t lineEnd = str.find('\n', pos);
            if (lineEnd == std::string::npos) lineEnd = str.size();

            std::string line = str.substr(pos, lineEnd - pos);
            bool isBlank = std::all_of(line.begin(), line.end(), [](char c) {
                return c == ' ' || c == '\t' || c == '\r';
            });

            if (!isBlank)
            {
                start = pos;
                break;
            }
            pos = lineEnd + 1;
        }

        // Trim trailing blank lines
        if (end > start)
        {
            pos = end;
            while (pos > start)
            {
                size_t lineStart = str.rfind('\n', pos - 1);
                if (lineStart == std::string::npos) lineStart = 0;
                else lineStart++;

                std::string line = str.substr(lineStart, pos - lineStart);
                bool isBlank = std::all_of(line.begin(), line.end(), [](char c) {
                    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
                });

                if (!isBlank)
                {
                    end = pos;
                    break;
                }
                pos = (lineStart > 0) ? lineStart - 1 : 0;
                if (pos == 0 && lineStart == 0) break;
            }
        }

        return str.substr(start, end - start);
    }

    CoreShaderSource::CoreShaderSource(const std::string& rawSource)
        : ShaderSource(rawSource), m_RawSource(rawSource)
    {
        m_ParsedShaders = parseShaders(rawSource);
    }

    std::string CoreShaderSource::GetSource(const std::string& key)
    {
        auto it = m_ParsedShaders.find(key);
        if (it != m_ParsedShaders.end())
        {
            return it->second;
        }
        return "";
    }

    std::unordered_map<std::string, std::string> CoreShaderSource::parseShaders(const std::string& source)
    {
        std::unordered_map<std::string, std::string> shaders;

        std::istringstream stream(source);
        std::string line;

        std::string currentKey;
        std::ostringstream currentSource;

        const std::string headerPrefix = "### ";
        const std::string headerSuffix = " ###";

        auto flushCurrent = [&]()
        {
            if (!currentKey.empty())
            {
                shaders[currentKey] = trimBlankLines(currentSource.str());
                currentSource.str({});
                currentSource.clear();
            }
        };

        while (std::getline(stream, line))
        {
            std::string trimmedLine = trimLeadingWhitespace(line);

            if (startsWith(trimmedLine, headerPrefix) && endsWith(trimmedLine, headerSuffix))
            {
                flushCurrent();

                currentKey = trimmedLine.substr(
                    headerPrefix.size(),
                    trimmedLine.size() - headerPrefix.size() - headerSuffix.size()
                );
            }
            else if (!currentKey.empty())
            {
                currentSource << trimmedLine << '\n';
            }
        }

        flushCurrent();
        return shaders;
    }

}

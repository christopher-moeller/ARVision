#pragma once

#include "MathTypes.h"
#include <vector>
#include <string>

class OutputWriter {
public:
    static bool writeLines(const std::string& filepath, const std::vector<Line2D>& lines);
};

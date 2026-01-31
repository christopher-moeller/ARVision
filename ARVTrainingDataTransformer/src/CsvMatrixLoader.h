#pragma once

#include "MathTypes.h"
#include <string>

class CsvMatrixLoader {
public:
    static bool load(const std::string& filepath, Mat4& matrix);
};

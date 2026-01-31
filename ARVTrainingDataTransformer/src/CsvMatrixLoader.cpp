#include "CsvMatrixLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool CsvMatrixLoader::load(const std::string& filepath, Mat4& matrix) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open MVP file: " << filepath << std::endl;
        return false;
    }

    // Initialize to identity
    matrix = Mat4(1.0f);

    std::string line;
    int row = 0;

    while (std::getline(file, line) && row < 4) {
        std::istringstream iss(line);
        std::string token;
        int col = 0;

        while (std::getline(iss, token, ',') && col < 4) {
            float value = std::stof(token);
            // GLM uses column-major storage: matrix[col][row]
            // CSV is row-major, so we read row by row
            matrix[col][row] = value;
            col++;
        }
        row++;
    }

    file.close();

    if (row != 4) {
        std::cerr << "Error: Invalid MVP matrix format in " << filepath << std::endl;
        return false;
    }

    return true;
}

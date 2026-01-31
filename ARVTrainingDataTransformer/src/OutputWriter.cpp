#include "OutputWriter.h"
#include <fstream>
#include <iomanip>
#include <iostream>

bool OutputWriter::writeLines(const std::string& filepath, const std::vector<Line2D>& lines) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file: " << filepath << std::endl;
        return false;
    }

    // Set precision for floating point output
    file << std::fixed << std::setprecision(2);

    for (const auto& line : lines) {
        file << line.x1 << "," << line.y1 << "," << line.x2 << "," << line.y2 << "\n";
    }

    file.close();
    return true;
}

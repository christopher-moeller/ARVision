#include "Renderer.h"
#include <iostream>
#include <filesystem>
#include <regex>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

struct FrameInfo {
    int frameNumber;
    std::string mvpPath;
    std::string screenshotPath;
};

std::vector<FrameInfo> discoverFrames(const std::string& inputDir) {
    std::vector<FrameInfo> frames;
    std::regex mvpPattern(R"(mvp_(\d+)\.csv)");

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (!entry.is_regular_file()) continue;

        std::string filename = entry.path().filename().string();
        std::smatch match;

        if (std::regex_match(filename, match, mvpPattern)) {
            int frameNum = std::stoi(match[1].str());

            // Build screenshot path
            std::ostringstream ssPath;
            ssPath << inputDir << "/screenshot_" << frameNum << ".png";
            std::string screenshotPath = ssPath.str();

            // Check if screenshot exists
            if (fs::exists(screenshotPath)) {
                FrameInfo info;
                info.frameNumber = frameNum;
                info.mvpPath = entry.path().string();
                info.screenshotPath = screenshotPath;
                frames.push_back(info);
            }
        }
    }

    // Sort by frame number
    std::sort(frames.begin(), frames.end(),
              [](const FrameInfo& a, const FrameInfo& b) { return a.frameNumber < b.frameNumber; });

    return frames;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <input_directory> [output_directory]" << std::endl;
    std::cout << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  input_directory   Directory containing 3d_scene.obj, mvp_*.csv, and screenshot_*.png files" << std::endl;
    std::cout << "  output_directory  Directory for output files (default: input_directory/output)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputDir = argv[1];
    std::string outputDir = (argc > 2) ? argv[2] : (inputDir + "/output");

    // Validate input directory
    if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
        std::cerr << "Error: Input directory does not exist: " << inputDir << std::endl;
        return 1;
    }

    // Create output directory if needed
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    // Check for OBJ file
    std::string objPath = inputDir + "/3d_scene.obj";
    if (!fs::exists(objPath)) {
        std::cerr << "Error: OBJ file not found: " << objPath << std::endl;
        return 1;
    }

    // Discover frames
    std::vector<FrameInfo> frames = discoverFrames(inputDir);
    if (frames.empty()) {
        std::cerr << "Error: No frames found in input directory" << std::endl;
        return 1;
    }

    std::cout << "Found " << frames.size() << " frames" << std::endl;

    // Create renderer and load mesh
    Renderer renderer;
    if (!renderer.loadMesh(objPath)) {
        std::cerr << "Error: Failed to load mesh" << std::endl;
        return 1;
    }

    // Process each frame
    int processedCount = 0;
    int errorCount = 0;

    for (const auto& frame : frames) {
        std::ostringstream outputPath;
        outputPath << outputDir << "/frame_" << frame.frameNumber << "_lines.csv";

        std::cout << "\n--- Frame " << frame.frameNumber << " ---" << std::endl;

        if (renderer.processFrame(frame.mvpPath, frame.screenshotPath, outputPath.str())) {
            processedCount++;
        } else {
            std::cerr << "Error processing frame " << frame.frameNumber << std::endl;
            errorCount++;
        }
    }

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Processed: " << processedCount << " frames" << std::endl;
    std::cout << "Errors: " << errorCount << " frames" << std::endl;

    return (errorCount > 0) ? 1 : 0;
}

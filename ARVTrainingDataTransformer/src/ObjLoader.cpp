#include "ObjLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool ObjLoader::load(const std::string& filepath, Mesh& mesh) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open OBJ file: " << filepath << std::endl;
        return false;
    }

    mesh.vertices.clear();
    mesh.triangles.clear();

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            // Vertex position
            float x, y, z;
            iss >> x >> y >> z;
            mesh.vertices.push_back(Vec3(x, y, z));
        }
        else if (prefix == "f") {
            // Face - can be triangle or quad
            std::vector<int> indices;
            std::string token;

            while (iss >> token) {
                // Parse vertex index (handle v, v/vt, v/vt/vn, v//vn formats)
                int vertexIndex = 0;
                size_t slashPos = token.find('/');
                if (slashPos != std::string::npos) {
                    vertexIndex = std::stoi(token.substr(0, slashPos));
                } else {
                    vertexIndex = std::stoi(token);
                }

                // Convert from 1-based to 0-based indexing
                indices.push_back(vertexIndex - 1);
            }

            // Triangulate: first triangle
            if (indices.size() >= 3) {
                mesh.triangles.push_back({indices[0], indices[1], indices[2]});
            }

            // Triangulate: additional triangles for quads and polygons
            for (size_t i = 3; i < indices.size(); ++i) {
                mesh.triangles.push_back({indices[0], indices[i-1], indices[i]});
            }
        }
    }

    file.close();

    std::cout << "Loaded OBJ: " << mesh.vertices.size() << " vertices, "
              << mesh.triangles.size() << " triangles" << std::endl;

    return true;
}

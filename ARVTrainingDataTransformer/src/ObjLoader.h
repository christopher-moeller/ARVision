#pragma once

#include "MathTypes.h"
#include <vector>
#include <string>

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<Triangle> triangles;
};

class ObjLoader {
public:
    static bool load(const std::string& filepath, Mesh& mesh);
};

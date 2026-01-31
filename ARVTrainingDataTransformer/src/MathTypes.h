#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <algorithm>
#include <functional>

using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;

struct Triangle {
    int v0, v1, v2;
};

struct Line2D {
    float x1, y1, x2, y2;

    bool operator==(const Line2D& other) const {
        return (x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2) ||
               (x1 == other.x2 && y1 == other.y2 && x2 == other.x1 && y2 == other.y1);
    }
};

struct Edge {
    int v0, v1;

    Edge(int a, int b) : v0(std::min(a, b)), v1(std::max(a, b)) {}

    bool operator==(const Edge& other) const {
        return v0 == other.v0 && v1 == other.v1;
    }
};

namespace std {
    template<>
    struct hash<Edge> {
        size_t operator()(const Edge& e) const {
            return hash<int>()(e.v0) ^ (hash<int>()(e.v1) << 16);
        }
    };
}

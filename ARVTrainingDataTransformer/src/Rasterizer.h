#pragma once

#include "MathTypes.h"
#include <vector>
#include <limits>

class Rasterizer {
public:
    Rasterizer(int width, int height);

    void clear();
    void rasterizeTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2);

    float getDepth(int x, int y) const;
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    int m_width;
    int m_height;
    std::vector<float> m_depthBuffer;

    void rasterizeTriangleBarycentric(const Vec3& v0, const Vec3& v1, const Vec3& v2);
    float edgeFunction(const Vec2& a, const Vec2& b, const Vec2& c) const;
};

#include "Rasterizer.h"
#include <algorithm>
#include <cmath>

Rasterizer::Rasterizer(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_depthBuffer(width * height, std::numeric_limits<float>::infinity())
{
}

void Rasterizer::clear() {
    std::fill(m_depthBuffer.begin(), m_depthBuffer.end(), std::numeric_limits<float>::infinity());
}

float Rasterizer::getDepth(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return std::numeric_limits<float>::infinity();
    }
    return m_depthBuffer[y * m_width + x];
}

float Rasterizer::edgeFunction(const Vec2& a, const Vec2& b, const Vec2& c) const {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void Rasterizer::rasterizeTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2) {
    rasterizeTriangleBarycentric(v0, v1, v2);
}

void Rasterizer::rasterizeTriangleBarycentric(const Vec3& v0, const Vec3& v1, const Vec3& v2) {
    // Compute bounding box
    int minX = static_cast<int>(std::floor(std::min({v0.x, v1.x, v2.x})));
    int maxX = static_cast<int>(std::ceil(std::max({v0.x, v1.x, v2.x})));
    int minY = static_cast<int>(std::floor(std::min({v0.y, v1.y, v2.y})));
    int maxY = static_cast<int>(std::ceil(std::max({v0.y, v1.y, v2.y})));

    // Clip to screen bounds
    minX = std::max(0, minX);
    maxX = std::min(m_width - 1, maxX);
    minY = std::max(0, minY);
    maxY = std::min(m_height - 1, maxY);

    Vec2 p0(v0.x, v0.y);
    Vec2 p1(v1.x, v1.y);
    Vec2 p2(v2.x, v2.y);

    // Compute the area of the triangle (for normalization)
    float area = edgeFunction(p0, p1, p2);

    // Skip degenerate triangles
    if (std::abs(area) < 1e-6f) {
        return;
    }

    // Rasterize
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            Vec2 p(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);

            // Compute barycentric coordinates
            float w0 = edgeFunction(p1, p2, p);
            float w1 = edgeFunction(p2, p0, p);
            float w2 = edgeFunction(p0, p1, p);

            // Check if pixel is inside triangle
            bool inside = (area > 0) ? (w0 >= 0 && w1 >= 0 && w2 >= 0) : (w0 <= 0 && w1 <= 0 && w2 <= 0);

            if (inside) {
                // Normalize barycentric coordinates
                w0 /= area;
                w1 /= area;
                w2 /= area;

                // Interpolate depth
                float depth = w0 * v0.z + w1 * v1.z + w2 * v2.z;

                // Depth test
                int idx = y * m_width + x;
                if (depth < m_depthBuffer[idx]) {
                    m_depthBuffer[idx] = depth;
                }
            }
        }
    }
}

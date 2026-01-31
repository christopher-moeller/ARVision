#include "Visibility.h"
#include <cmath>
#include <algorithm>
#include <unordered_set>

Visibility::Visibility(const Rasterizer& rasterizer, float depthEpsilon, float visibilityThreshold)
    : m_rasterizer(rasterizer)
    , m_depthEpsilon(depthEpsilon)
    , m_visibilityThreshold(visibilityThreshold)
    , m_sampleCount(20)  // Number of samples along each edge
{
}

bool Visibility::isPointVisible(float x, float y, float depth) const {
    int ix = static_cast<int>(x);
    int iy = static_cast<int>(y);

    // Check bounds
    if (ix < 0 || ix >= m_rasterizer.getWidth() || iy < 0 || iy >= m_rasterizer.getHeight()) {
        return false;
    }

    float bufferDepth = m_rasterizer.getDepth(ix, iy);
    return depth <= (bufferDepth + m_depthEpsilon);
}

bool Visibility::isEdgeVisible(const Vec3& v0, const Vec3& v1) const {
    int visibleSamples = 0;

    for (int i = 0; i <= m_sampleCount; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(m_sampleCount);

        // Interpolate position and depth
        float x = v0.x + t * (v1.x - v0.x);
        float y = v0.y + t * (v1.y - v0.y);
        float z = v0.z + t * (v1.z - v0.z);

        if (isPointVisible(x, y, z)) {
            visibleSamples++;
        }
    }

    // Edge is visible if enough samples pass the visibility test
    float visibleRatio = static_cast<float>(visibleSamples) / static_cast<float>(m_sampleCount + 1);
    return visibleRatio >= m_visibilityThreshold;
}

std::vector<Line2D> Visibility::filterVisibleEdges(const std::vector<Vec3>& screenVertices,
                                                    const std::vector<Edge>& edges) const {
    std::vector<Line2D> visibleLines;

    for (const auto& edge : edges) {
        if (edge.v0 < 0 || edge.v0 >= static_cast<int>(screenVertices.size()) ||
            edge.v1 < 0 || edge.v1 >= static_cast<int>(screenVertices.size())) {
            continue;
        }

        const Vec3& v0 = screenVertices[edge.v0];
        const Vec3& v1 = screenVertices[edge.v1];

        // Skip edges where both endpoints are outside the screen
        bool v0InScreen = v0.x >= 0 && v0.x < m_rasterizer.getWidth() &&
                          v0.y >= 0 && v0.y < m_rasterizer.getHeight();
        bool v1InScreen = v1.x >= 0 && v1.x < m_rasterizer.getWidth() &&
                          v1.y >= 0 && v1.y < m_rasterizer.getHeight();

        if (!v0InScreen && !v1InScreen) {
            continue;
        }

        if (isEdgeVisible(v0, v1)) {
            Line2D line;
            line.x1 = v0.x;
            line.y1 = v0.y;
            line.x2 = v1.x;
            line.y2 = v1.y;
            visibleLines.push_back(line);
        }
    }

    return visibleLines;
}

#pragma once

#include "MathTypes.h"
#include "Rasterizer.h"
#include <vector>

class Visibility {
public:
    Visibility(const Rasterizer& rasterizer, float depthEpsilon = 0.02f, float visibilityThreshold = 0.5f);

    bool isEdgeVisible(const Vec3& v0, const Vec3& v1) const;
    std::vector<Line2D> filterVisibleEdges(const std::vector<Vec3>& screenVertices,
                                            const std::vector<Edge>& edges) const;

private:
    const Rasterizer& m_rasterizer;
    float m_depthEpsilon;
    float m_visibilityThreshold;
    int m_sampleCount;

    bool isPointVisible(float x, float y, float depth) const;
};

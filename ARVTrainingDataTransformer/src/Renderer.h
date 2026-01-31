#pragma once

#include "MathTypes.h"
#include "ObjLoader.h"
#include "CsvMatrixLoader.h"
#include "Rasterizer.h"
#include "Visibility.h"
#include "OutputWriter.h"
#include <vector>
#include <string>
#include <unordered_set>

class Renderer {
public:
    Renderer();

    bool loadMesh(const std::string& objPath);
    bool processFrame(const std::string& mvpPath, const std::string& screenshotPath,
                      const std::string& outputPath);

private:
    Mesh m_mesh;
    std::vector<Edge> m_edges;

    void extractEdges();

    // Transform vertices from world space to clip space
    std::vector<Vec4> transformToClipSpace(const Mat4& mvp) const;

    // Perform perspective divide (clip -> NDC)
    std::vector<Vec3> perspectiveDivide(const std::vector<Vec4>& clipVertices) const;

    // Convert NDC to screen space
    std::vector<Vec3> ndcToScreen(const std::vector<Vec3>& ndcVertices, int width, int height) const;

    // Check if a vertex is inside the frustum
    bool isInsideFrustum(const Vec3& ndc) const;

    // Get triangles that are potentially visible
    std::vector<int> getVisibleTriangleIndices(const std::vector<Vec3>& ndcVertices) const;
};

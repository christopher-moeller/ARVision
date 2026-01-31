#include "Renderer.h"
#include <iostream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Renderer::Renderer() {
}

bool Renderer::loadMesh(const std::string& objPath) {
    if (!ObjLoader::load(objPath, m_mesh)) {
        return false;
    }
    extractEdges();
    std::cout << "Extracted " << m_edges.size() << " unique edges" << std::endl;
    return true;
}

void Renderer::extractEdges() {
    std::unordered_set<Edge> edgeSet;

    for (const auto& tri : m_mesh.triangles) {
        edgeSet.insert(Edge(tri.v0, tri.v1));
        edgeSet.insert(Edge(tri.v1, tri.v2));
        edgeSet.insert(Edge(tri.v2, tri.v0));
    }

    m_edges.assign(edgeSet.begin(), edgeSet.end());
}

std::vector<Vec4> Renderer::transformToClipSpace(const Mat4& mvp) const {
    std::vector<Vec4> clipVertices;
    clipVertices.reserve(m_mesh.vertices.size());

    for (const auto& v : m_mesh.vertices) {
        Vec4 clip = mvp * Vec4(v.x, v.y, v.z, 1.0f);
        clipVertices.push_back(clip);
    }

    return clipVertices;
}

std::vector<Vec3> Renderer::perspectiveDivide(const std::vector<Vec4>& clipVertices) const {
    std::vector<Vec3> ndcVertices;
    ndcVertices.reserve(clipVertices.size());

    for (const auto& clip : clipVertices) {
        if (std::abs(clip.w) < 1e-6f) {
            // Avoid division by zero
            ndcVertices.push_back(Vec3(0.0f, 0.0f, 1.0f));
        } else {
            ndcVertices.push_back(Vec3(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w));
        }
    }

    return ndcVertices;
}

std::vector<Vec3> Renderer::ndcToScreen(const std::vector<Vec3>& ndcVertices, int width, int height) const {
    std::vector<Vec3> screenVertices;
    screenVertices.reserve(ndcVertices.size());

    float w = static_cast<float>(width);
    float h = static_cast<float>(height);

    for (const auto& ndc : ndcVertices) {
        float screenX = (ndc.x * 0.5f + 0.5f) * w;
        float screenY = (1.0f - (ndc.y * 0.5f + 0.5f)) * h;
        screenVertices.push_back(Vec3(screenX, screenY, ndc.z));
    }

    return screenVertices;
}

bool Renderer::isInsideFrustum(const Vec3& ndc) const {
    return ndc.x >= -1.0f && ndc.x <= 1.0f &&
           ndc.y >= -1.0f && ndc.y <= 1.0f &&
           ndc.z >= -1.0f && ndc.z <= 1.0f;
}

std::vector<int> Renderer::getVisibleTriangleIndices(const std::vector<Vec3>& ndcVertices) const {
    std::vector<int> visibleIndices;

    for (size_t i = 0; i < m_mesh.triangles.size(); ++i) {
        const auto& tri = m_mesh.triangles[i];

        // Check if at least one vertex is inside the frustum
        bool v0Inside = isInsideFrustum(ndcVertices[tri.v0]);
        bool v1Inside = isInsideFrustum(ndcVertices[tri.v1]);
        bool v2Inside = isInsideFrustum(ndcVertices[tri.v2]);

        if (v0Inside || v1Inside || v2Inside) {
            visibleIndices.push_back(static_cast<int>(i));
        }
    }

    return visibleIndices;
}

bool Renderer::processFrame(const std::string& mvpPath, const std::string& screenshotPath,
                            const std::string& outputPath) {
    // Load MVP matrix
    Mat4 mvp;
    if (!CsvMatrixLoader::load(mvpPath, mvp)) {
        return false;
    }

    // Load screenshot to get dimensions
    int width, height, channels;
    unsigned char* imageData = stbi_load(screenshotPath.c_str(), &width, &height, &channels, 0);
    if (!imageData) {
        std::cerr << "Error: Could not load screenshot: " << screenshotPath << std::endl;
        return false;
    }
    stbi_image_free(imageData);

    std::cout << "Processing frame: " << width << "x" << height << std::endl;

    // Step 1: Transform vertices to clip space
    std::vector<Vec4> clipVertices = transformToClipSpace(mvp);

    // Step 2: Perspective divide (clip -> NDC)
    std::vector<Vec3> ndcVertices = perspectiveDivide(clipVertices);

    // Step 3: Get visible triangles
    std::vector<int> visibleTriIndices = getVisibleTriangleIndices(ndcVertices);
    std::cout << "Visible triangles: " << visibleTriIndices.size() << " / " << m_mesh.triangles.size() << std::endl;

    // Step 4: Convert NDC to screen space
    std::vector<Vec3> screenVertices = ndcToScreen(ndcVertices, width, height);

    // Step 5: Rasterize visible triangles to depth buffer
    Rasterizer rasterizer(width, height);

    for (int triIdx : visibleTriIndices) {
        const auto& tri = m_mesh.triangles[triIdx];
        rasterizer.rasterizeTriangle(screenVertices[tri.v0],
                                      screenVertices[tri.v1],
                                      screenVertices[tri.v2]);
    }

    // Step 6: Extract edges from visible triangles
    std::unordered_set<Edge> visibleEdgeSet;
    for (int triIdx : visibleTriIndices) {
        const auto& tri = m_mesh.triangles[triIdx];
        visibleEdgeSet.insert(Edge(tri.v0, tri.v1));
        visibleEdgeSet.insert(Edge(tri.v1, tri.v2));
        visibleEdgeSet.insert(Edge(tri.v2, tri.v0));
    }
    std::vector<Edge> visibleEdges(visibleEdgeSet.begin(), visibleEdgeSet.end());

    // Step 7: Filter edges by visibility
    Visibility visibility(rasterizer);
    std::vector<Line2D> visibleLines = visibility.filterVisibleEdges(screenVertices, visibleEdges);

    std::cout << "Visible edges: " << visibleLines.size() << " / " << visibleEdges.size() << std::endl;

    // Step 8: Write output
    if (!OutputWriter::writeLines(outputPath, visibleLines)) {
        return false;
    }

    std::cout << "Written output to: " << outputPath << std::endl;
    return true;
}

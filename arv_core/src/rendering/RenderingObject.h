#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include "rendering/Shader.h"
#include "rendering/VertexArray.h"
#include "rendering/Texture.h"
#include "rendering/CoreShaderSource.h"

namespace arv {

    class Renderer;

    class RenderingObject {

    public:
        virtual ~RenderingObject() = default;

        virtual std::shared_ptr<Shader>& GetShader() = 0;
        virtual std::shared_ptr<VertexArray>& GetVertexArray() = 0;
        virtual std::shared_ptr<Texture2D> GetTexture() { return nullptr; }

        // Returns vertex positions as vec3 (x, y, z) in local/model space
        virtual const std::vector<glm::vec3>& GetMeshVertices() const { return s_EmptyVertices; }
        // Returns triangle indices (3 indices per triangle)
        virtual const std::vector<uint32_t>& GetMeshIndices() const { return s_EmptyIndices; }

        glm::vec3& GetPosition() { return position; }
        const glm::vec3& GetPosition() const { return position; }
        void SetPosition(const glm::vec3& pos) { position = pos; }
        
        const std::string& GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

        glm::vec3& GetScale() { return m_scale; }
        const glm::vec3& GetScale() const { return m_scale; }
        void SetScale(const glm::vec3& scale) { m_scale = scale; }

        glm::vec3& GetRotation() { return m_rotation; }
        const glm::vec3& GetRotation() const { return m_rotation; }
        void SetRotation(const glm::vec3& rotation) { m_rotation = rotation; }

        const glm::vec3& GetBoundsMin() const { return m_boundsMin; }
        const glm::vec3& GetBoundsMax() const { return m_boundsMax; }
        glm::vec3 GetBoundsSize() const { return m_boundsMax - m_boundsMin; }
        glm::vec3 GetBoundsCenter() const { return (m_boundsMin + m_boundsMax) * 0.5f; }

        virtual void RenderCustomImGui() {}
        virtual void SaveCustomProperties(nlohmann::json& j) const {}

    protected:
        // Helper struct for common rendering resources
        struct RenderingResources {
            std::unique_ptr<CoreShaderSource> shaderSource;
            std::shared_ptr<Shader> shader;
            std::shared_ptr<VertexArray> vertexArray;
        };

        // Creates shader and vertex array from source code and geometry data
        // This eliminates ~70% boilerplate across RenderingObject subclasses
        static RenderingResources SetupRendering(
            Renderer* renderer,
            const std::string& shaderSourceCode,
            const float* vertices, size_t verticesSizeBytes,
            const uint32_t* indices, size_t indexCount,
            const BufferLayout& layout);

        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 m_scale{1.0f, 1.0f, 1.0f};
        glm::vec3 m_rotation{0.0f, 0.0f, 0.0f};
        std::string m_name;
        glm::vec3 m_boundsMin{0.0f};
        glm::vec3 m_boundsMax{0.0f};

    private:
        static const std::vector<glm::vec3> s_EmptyVertices;
        static const std::vector<uint32_t> s_EmptyIndices;
    };

}

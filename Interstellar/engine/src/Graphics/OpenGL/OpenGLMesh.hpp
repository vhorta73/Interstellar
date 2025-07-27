#pragma once

#include "Interstellar/Graphics/Core/IMesh.hpp"

namespace Interstellar::Graphics::OpenGL {

    class OpenGLMesh : public Core::IMesh {
    public:
        OpenGLMesh(const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize);
        ~OpenGLMesh();

        const char* GetDebugName() const override;
        size_t GetVertexCount() const override;
        size_t GetIndexCount() const override;
        void* GetNativeHandle() const override;

    private:
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;
        unsigned int m_EBO = 0;

        size_t m_VertexCount = 0;
        size_t m_IndexCount = 0;
    };

}

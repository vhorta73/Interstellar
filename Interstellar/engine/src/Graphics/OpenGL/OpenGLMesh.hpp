#pragma once

#include <string>

#include "Interstellar/Graphics/Core/IMesh.hpp"

namespace Interstellar::Graphics::OpenGL {

    /// Represents a mesh stored in OpenGL buffers (VAO/VBO/EBO)
    class OpenGLMesh : public Core::IMesh {
    public:
        /// <summary>
        /// Creates a mesh given raw vertex and index data.
        /// </summary>
        /// <param name="vertexData"></param>
        /// <param name="vertexSize"></param>
        /// <param name="indexData"></param>
        /// <param name="indexSize"></param>
        OpenGLMesh(const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize);

        /// <summary>
        /// Cleans up OpenGL resources.
        /// </summary>
        ~OpenGLMesh();

        /// <summary>
        /// Returns the debug name of the mesh.
        /// </summary>
        /// <returns>A string view of the debug name.</returns>
        std::string_view GetDebugName() const override;

        /// <summary>
        /// Returns the number of vertices.
        /// </summary>
        /// <returns>Total vertex count.</returns>
        size_t GetVertexCount() const override;

        /// <summary>
        /// Returns the number of indices.
        /// </summary>
        /// <returns>The total count of indices as a size_t value.</returns>
        size_t GetIndexCount() const override;

        /// <summary>
        /// Returns the native OpenGL handle (VAO).
        /// </summary>
        /// <returns>Pointer to the OpenGL VAO.</returns>
        void* GetNativeHandle() const override;

        /// <summary>
        /// Bind the mesh for rendering.
        /// </summary>
        void Bind() const;

    private:
        unsigned int m_VAO = 0;
        unsigned int m_VBO = 0;
        unsigned int m_EBO = 0;

        size_t m_VertexCount = 0;
        size_t m_IndexCount = 0;

        std::string m_DebugName = "OpenGLMesh"; ///< Optional debug label.
    };
}

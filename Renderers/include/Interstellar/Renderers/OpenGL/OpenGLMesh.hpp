#pragma once

#include <string>

#include "Interstellar/Graphics/IMesh.hpp"

namespace Interstellar::Renderers::OpenGL {
    /**
    * @class OpenGLMesh
    * @brief Concrete implementation of IMesh for the OpenGL backend.
    *
    * Stores vertex and index data using OpenGL VAO, VBO, and EBO.
    *
    * @since 1.0
    */
    class OpenGLMesh : public Interstellar::Graphics::IMesh {
    public:
        /**
        * @brief Creates a mesh given raw vertex and index data.
        * @param vertexData Pointer to the raw vertex data.
        * @param vertexSize Size in bytes of the vertex buffer.
        * @param indexData Pointer to the raw index buffer.
        * @param indexSize Size in bytes of the index buffer.
        * 
        * @since 1.0
        */
        OpenGLMesh(const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize);

        /**
        * @brief Cleans up OpenGL resources.
        */
        ~OpenGLMesh();

        /**
        * @brief Returns the debug name of the mesh.
        *
        * @return A string view of the debug name.
        * @since 1.0
        * @see SetDebugName()
        */
        std::string_view GetDebugName() const override;

        /**
         * @brief Sets the debug name of the mesh.
         * This implementation always returns @a true since OpenGLMesh supports debug names.
         *
         * @param name The new debug name string.
         * @return True if the debug name was set successfully.
         * @since 1.0
         */
        bool SetDebugName(std::string name) override;


        /**
        * @brief Returns the number of vertices.
        * @return Total vertex count.
        * @since 1.0
        */
        size_t GetVertexCount() const override;

        /**
        * @brief Returns the number of indices.
        * 
        * @return The total count of indices as a size_t value.
        * @since 1.0
        */
        size_t GetIndexCount() const override;

        /**
        * @brief Returns the native OpenGL handle (VAO).
        * 
        * @return Pointer to the OpenGL VAO.
        * @since 1.0
        */
        void* GetNativeHandle() const override;

        /**
        * @brief Bind the mesh for rendering.
        * @since 1.0
        * @see GetNativeHandle()
        */
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

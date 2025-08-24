#pragma once

#include <cstddef>   // size_t
#include <string>

#include "Interstellar/Graphics/IMesh.hpp"

/**
 * @file
 * @ingroup Renderers
 * @brief OpenGL mesh implementation header.
 * @details
 *   Concrete mesh backed by OpenGL VAO/VBO/EBO objects. Exposes counts,
 *   a debug name, an opaque native handle, and a convenience Bind().
 *
 *   Notes:
 *   - The native handle returned by GetNativeHandle() is opaque. For OpenGL,
 *     implementations typically encode the VAO name into a pointer value.
 *     Do not dereference; only use it in backend-specific code.
 */

 /**
  * @defgroup GraphicsOpenGL OpenGL Backend
  * @ingroup Renderers
  * @brief Types provided by the OpenGL renderer.
  * @since 1.0
  */

namespace Interstellar::Renderers::OpenGL {

    /**
     * @brief Concrete IMesh for the OpenGL backend.
     * @ingroup GraphicsOpenGL
     * @details
     *   Stores vertex/index data using OpenGL buffers and a VAO.
     *   Unless otherwise documented, all methods are expected to be called
     *   on the render/main thread after a valid GL context is current.
     *
     * @since 1.0
     * @see Interstellar::Graphics::IMesh
     */
    class OpenGLMesh : public Interstellar::Graphics::IMesh {
    public:
        /**
         * @brief Construct a mesh from raw vertex and index data.
         * @param vertexData Pointer to vertex data (must be non-null if vertexSize > 0).
         * @param vertexSize Size in bytes of vertex buffer (>= 0).
         * @param indexData  Pointer to index data (may be null when indexSize == 0).
         * @param indexSize  Size in bytes of index buffer (0 for non-indexed).
         * @pre A valid OpenGL context is current on the calling thread.
         * @since 1.0
         */
        OpenGLMesh(const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize);

        /**
         * @brief Destroy GL resources (VAO/VBO/EBO) if owned.
         * @since 1.0
         */
        ~OpenGLMesh();

        /**
         * @brief Get the debug name of the mesh.
         * @return String view of the current debug label (may be empty).
         * @since 1.0
         * @see SetDebugName()
         */
        std::string_view GetDebugName() const override;

        /**
         * @brief Set a debug name for the mesh.
         * @details This implementation supports debug names and returns true.
         * @param name New debug label.
         * @return true on success.
         * @since 1.0
         */
        virtual bool SetDebugName(std::string_view& name) override;

        /**
         * @brief Number of vertices.
         * @return Total vertex count.
         * @since 1.0
         */
        size_t GetVertexCount() const override;

        /**
         * @brief Number of indices.
         * @return Total index count (0 for non-indexed meshes).
         * @since 1.0
         */
        size_t GetIndexCount() const override;

        /**
         * @brief Backend-native handle (opaque).
         * @details For OpenGL, this typically encodes the VAO name into a pointer.
         *          Do not dereference. Intended only for backend interop.
         * @return Opaque pointer representing the VAO handle.
         * @since 1.0
         */
        void* GetNativeHandle() const override;

        /**
         * @brief Bind the mesh for drawing (binds VAO; VBO/EBO are part of VAO state).
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

        std::string m_DebugName = "OpenGLMesh";
    };

} // namespace Interstellar::Renderers::OpenGL

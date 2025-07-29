#include "OpenGLMesh.hpp"
#include <glad/glad.h>

using namespace Interstellar::Graphics::OpenGL;

/**
 * @brief Constructs an OpenGL mesh and uploads vertex/index data to GPU buffers.
 *
 * This implementation assumes a fixed vertex layout of:
 * - 3 floats for position (x, y, z)
 * - 2 floats for texture coordinates (u, v)
 *
 * This layout totals 5 floats per vertex and assigns:
 * - Location 0 -> vec3 position
 * - Location 1 -> vec2 UVs
 *
 * @param vertexData Raw pointer to the vertex buffer.
 * @param vertexSize Total size of the vertex buffer in bytes.
 * @param indexData Raw pointer to the index buffer.
 * @param indexSize Total size of the index buffer in bytes.
 */
OpenGLMesh::OpenGLMesh(const void* vertexData, size_t vertexSize,
    const void* indexData, size_t indexSize)
{
    m_VertexCount = vertexSize / (5 * sizeof(float)); // Each vertex = 3 pos + 2 UV
    m_IndexCount = indexSize / sizeof(unsigned int);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Upload vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    if (vertexData && vertexSize > 0) {
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
    }

    // Upload index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData, GL_STATIC_DRAW);

    // Vertex layout (interleaved): 3 floats for position, 2 for UV
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // UV
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind
}

/**
 * @brief Destructor. Releases OpenGL VAO/VBO/EBO resources.
 */
OpenGLMesh::~OpenGLMesh() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

/**
 * @brief Returns the human-readable debug name for this mesh.
 * @return The debug label string.
 */
std::string_view OpenGLMesh::GetDebugName() const {
    return m_DebugName;
}

/**
 * @brief Sets the debug name for this mesh.
 * @param name New debug name.
 * @return Always true for OpenGLMesh.
 */
bool OpenGLMesh::SetDebugName(std::string name) {
    m_DebugName = std::move(name);
    return true;
}

/**
 * @brief Returns the number of vertices.
 * @return Vertex count.
 */
size_t OpenGLMesh::GetVertexCount() const {
    return m_VertexCount;
}

/**
 * @brief Returns the number of indices.
 * @return Index count.
 */
size_t OpenGLMesh::GetIndexCount() const {
    return m_IndexCount;
}

/**
 * @brief Returns the OpenGL VAO ID as a void pointer.
 * @return Native handle to the mesh (VAO).
 */
void* OpenGLMesh::GetNativeHandle() const {
    return reinterpret_cast<void*>(static_cast<uintptr_t>(m_VAO));
}

/**
 * @brief Binds the VAO for this mesh to the current OpenGL context.
 */
void OpenGLMesh::Bind() const {
    glBindVertexArray(m_VAO);
}

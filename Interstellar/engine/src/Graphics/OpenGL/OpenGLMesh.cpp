#include "OpenGLMesh.hpp"
#include <glad/glad.h>

using namespace Interstellar::Graphics::OpenGL;

OpenGLMesh::OpenGLMesh(const void* vertexData, size_t vertexSize,
    const void* indexData, size_t indexSize)
{
    m_VertexCount = vertexSize / (3 * sizeof(float)); //assumes each vertex is a vec3 (x, y, z)
    m_IndexCount = indexSize / sizeof(unsigned int);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    if (vertexData && vertexSize > 0) {
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData, GL_STATIC_DRAW);

    // Vertex layout: location = 0, 3 floats per vertex (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // UV
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

OpenGLMesh::~OpenGLMesh() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

std::string_view OpenGLMesh::GetDebugName() const {
    return m_DebugName;
}

size_t OpenGLMesh::GetVertexCount() const {
    return m_VertexCount;
}

size_t OpenGLMesh::GetIndexCount() const {
    return m_IndexCount;
}

void* OpenGLMesh::GetNativeHandle() const {
    return reinterpret_cast<void*>(static_cast<uintptr_t>(m_VAO));
}

void OpenGLMesh::Bind() const {
    glBindVertexArray(m_VAO);
}

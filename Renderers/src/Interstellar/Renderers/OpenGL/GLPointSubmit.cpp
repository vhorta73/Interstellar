#include "Interstellar/Renderers/OpenGL/GLPointSubmit.hpp"

#include <glad/glad.h>
#include <cstdint>
#include <memory>

#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Graphics/IMaterial.hpp" // <-- includes FallbackMaterial

namespace Interstellar::Renderers::OpenGL {

    using namespace Interstellar::Graphics;

    namespace {

        // Push uniforms/textures from our concrete FallbackMaterial (same pattern as your GL path)
        inline void PushMaterialToGL(GLuint prog, const std::shared_ptr<IMaterial>& mat)
        {
            // Try concrete type used across the codebase
            if (auto* fb = dynamic_cast<FallbackMaterial*>(mat.get())) {
                // constants -> uniforms (size-based mapping)
                for (const auto& [name, bytes] : fb->GetConstants()) {
                    const GLint loc = glGetUniformLocation(prog, name.c_str());
                    if (loc < 0) continue;

                    switch (bytes.size()) {
                    case 4:   glUniform1fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // float
                    case 8:   glUniform2fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec2
                    case 12:  glUniform3fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec3
                    case 16:  glUniform4fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec4
                    case 64:  glUniformMatrix4fv(loc, 1, GL_FALSE, reinterpret_cast<const float*>(bytes.data())); break; // mat4
                    default:  /* extend as needed */ break;
                    }
                }

                // textures -> bind units + set sampler uniforms
                GLint unit = 0;
                for (const auto& [name, tex] : fb->GetTextures()) {
                    GLuint gltex = 0;
                    if (tex) {
                        gltex = static_cast<GLuint>(reinterpret_cast<uintptr_t>(tex->GetNativeHandle()));
                    }
                    if (!gltex) continue;

                    glActiveTexture(GL_TEXTURE0 + unit);
                    glBindTexture(GL_TEXTURE_2D, gltex);

                    if (GLint samplerLoc = glGetUniformLocation(prog, name.c_str()); samplerLoc >= 0) {
                        glUniform1i(samplerLoc, unit);
                    }
                    ++unit;
                }
            }
        }

        // Shared GL objects for point buffer
        static GLuint sVAO = 0;
        static GLuint sVBO = 0;

        inline void EnsureBuffers()
        {
            if (!sVAO) glGenVertexArrays(1, &sVAO);
            if (!sVBO) glGenBuffers(1, &sVBO);
        }

    } // namespace

    void GLPointSubmit::draw(IGraphics&,
        const std::shared_ptr<IRenderPipeline>& pipeline,
        const std::shared_ptr<IMaterial>& material,
        const float* xyPacked,
        int count)
    {
        if (!pipeline || !material || !xyPacked || count <= 0) return;

        // Bind program
        auto shader = pipeline->GetShader();
        const GLuint prog = static_cast<GLuint>(
            reinterpret_cast<uintptr_t>(shader ? shader->GetNativeHandle() : nullptr));
        glUseProgram(prog);

        // Push uniforms/textures from material
        PushMaterialToGL(prog, material);

        // Upload point positions (packed XY pairs)
        EnsureBuffers();
        glBindVertexArray(sVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sVBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(float) * 2 * count),
            xyPacked, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0); // layout(location=0) in vec2 a_StarPos;
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<void*>(0));

        // Draw as GL_POINTS
        glDrawArrays(GL_POINTS, 0, count);

        glBindVertexArray(0);
    }

} // namespace Interstellar::Renderers::OpenGL

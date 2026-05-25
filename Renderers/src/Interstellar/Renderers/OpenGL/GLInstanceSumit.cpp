#include "Interstellar/Renderers/OpenGL/GLInstancedSubmit.hpp"
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IMesh.hpp"
#include "Interstellar/Graphics/ITexture.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Renderers/OpenGL/GLPointSubmit.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLShader.hpp"

#include <glad/glad.h>
#include <cstdint>
#include <memory>

namespace {
    inline GLuint as_gl_uint(void* h) {
        return static_cast<GLuint>(reinterpret_cast<uintptr_t>(h));
    }

    // Very small, uniform/texture applier for the fallback material path.
    inline void apply_fallback_material(GLuint program,
        const std::shared_ptr<Interstellar::Graphics::IMaterial>& material) {
        using Interstellar::Graphics::FallbackMaterial;
        if (auto fb = std::dynamic_pointer_cast<FallbackMaterial>(material)) {
            // constants -> uniforms
            for (const auto& [name, bytes] : fb->GetConstants()) {
                if (bytes.empty()) continue;
                const GLint loc = glGetUniformLocation(program, name.c_str());
                if (loc < 0) continue;

                switch (bytes.size()) {
                case 4:  glUniform1fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // float
                case 8:  glUniform2fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec2
                case 12: glUniform3fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec3
                case 16: glUniform4fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec4
                case 64: glUniformMatrix4fv(loc, 1, GL_FALSE, reinterpret_cast<const float*>(bytes.data())); break; // mat4
                default: break; // extend as needed (ints, arrays, etc.)
                }
            }
            // textures -> bind to units 0..N, set sampler uniform to that unit
            GLint texUnit = 0;
            for (const auto& [name, tex] : fb->GetTextures()) {
                const GLuint gltex = as_gl_uint(tex ? tex->GetNativeHandle() : nullptr);
                if (!gltex) continue;
                glActiveTexture(GL_TEXTURE0 + texUnit);
                glBindTexture(GL_TEXTURE_2D, gltex);
                if (GLint samplerLoc = glGetUniformLocation(program, name.c_str()); samplerLoc >= 0) {
                    glUniform1i(samplerLoc, texUnit);
                }
                ++texUnit;
            }
        }
    }
}

namespace Interstellar::Renderers::OpenGL {

    static GLuint gVAO = 0, gVBO = 0;

    void GLInstancedSubmit::draw(Interstellar::Graphics::IGraphics& /*gfx*/,
        const std::shared_ptr<Interstellar::Graphics::IMesh>& mesh,
        const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
        const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
        const float* instanceXY,
        int instanceCount)
    {
        if (!mesh || !pipeline || !instanceXY || instanceCount <= 0) return;

        // Program
        auto shader = pipeline->GetShader();
        const GLuint prog = as_gl_uint(shader ? shader->GetNativeHandle() : nullptr);
        glUseProgram(prog);

        // Apply uniforms/textures (fallback material)
        apply_fallback_material(prog, material);

        // Bind VAO from mesh (OpenGL backend stores VAO in native handle)
        const GLuint vao = as_gl_uint(mesh->GetNativeHandle());
        glBindVertexArray(vao);

        // Create / update a single static VBO for per-instance XY
        static GLuint sInstanceVBO = 0;
        if (!sInstanceVBO) glGenBuffers(1, &sInstanceVBO);

        glBindBuffer(GL_ARRAY_BUFFER, sInstanceVBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(instanceCount) * 2 * sizeof(float),
            instanceXY, GL_DYNAMIC_DRAW);

        // Attribute location 2 -> vec2 i_Offset (per-instance)
        constexpr GLuint ATTR_LOC = 2;
        glEnableVertexAttribArray(ATTR_LOC);
        glVertexAttribPointer(ATTR_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
        glVertexAttribDivisor(ATTR_LOC, 1); // advance once per instance

        // Draw
        const GLsizei indexCount = static_cast<GLsizei>(mesh->GetIndexCount());
        glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr,
            static_cast<GLsizei>(instanceCount));

        // (leave state bound; your engine likely rebinding next draw anyway)
    }

} // namespace Interstellar::Renderers::OpenGL

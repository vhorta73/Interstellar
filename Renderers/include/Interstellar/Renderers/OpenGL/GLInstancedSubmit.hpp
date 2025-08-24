#pragma once
// Minimal helper to submit instanced draws without touching your IGraphics API.
// Uses attribute layout(location=2) for a vec2 per-instance offset.
//
// Usage:
//   GLInstancedSubmit::draw(*graphics, mesh, pipeline, material, offsets.data(), count);
//
// Offsets format: [x0, y0, x1, y1, ...] (float* length = count*2)

#include <cstddef>
#include <vector>
#include <glad/glad.h>
#include "Interstellar/Graphics/IMaterial.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IMesh.hpp"
#include "Interstellar/Graphics/ITexture.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLMesh.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLPipeline.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLShader.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"

namespace GLInstancedSubmit {

    // Reinterpret an opaque native handle to GLuint safely.
    inline GLuint as_gl_uint(void* h) {
        return static_cast<GLuint>(reinterpret_cast<uintptr_t>(h));
    }

    inline void draw(Interstellar::Renderers::OpenGL::OpenGLGraphics& /*gfx*/,
        const std::shared_ptr<Interstellar::Graphics::IMesh>& mesh,
        const std::shared_ptr<Interstellar::Graphics::IRenderPipeline>& pipeline,
        const std::shared_ptr<Interstellar::Graphics::IMaterial>& material,
        const float* instanceXY, std::size_t instanceCount)
    {
        using namespace Interstellar::Renderers::OpenGL;

        if (!mesh || !pipeline || !material || instanceCount == 0) return;

        auto glMesh = std::static_pointer_cast<OpenGLMesh>(mesh);
        auto glShader = std::static_pointer_cast<OpenGLShader>(pipeline->GetShader());

        // Bind shader/program
        const GLuint prog = as_gl_uint(glShader ? glShader->GetNativeHandle() : nullptr);
        glUseProgram(prog);

        // Apply material (fallback path in your OpenGLGraphics will read uniforms/textures)
        // We reuse the same code path by calling the pipeline-less SubmitMesh variant,
        // but here we set the per-instance attribute before the draw.
        glMesh->Bind(); // bind VAO owned by the mesh

        // Create & upload a transient instance buffer (XY pairs)
        static GLuint s_instanceVBO = 0;
        if (!s_instanceVBO) glGenBuffers(1, &s_instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, s_instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceCount * 2 * sizeof(float), instanceXY, GL_DYNAMIC_DRAW);

        // Instance attribute @ location=2: vec2 a_InstanceOffset
        constexpr GLuint ATTR_LOCATION = 2;
        glEnableVertexAttribArray(ATTR_LOCATION);
        glVertexAttribPointer(ATTR_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
        glVertexAttribDivisor(ATTR_LOCATION, 1); // advance per instance

        // Apply material: constants & textures (uniforms)
        if (auto fb = std::dynamic_pointer_cast<Interstellar::Graphics::FallbackMaterial>(material)) {
            // constants
            for (const auto& [name, bytes] : fb->GetConstants()) {
                GLint loc = glGetUniformLocation(prog, name.c_str());
                if (loc < 0) continue;
                switch (bytes.size()) {
                case 4:  glUniform1fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // float
                case 8:  glUniform2fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec2
                case 12: glUniform3fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec3
                case 16: glUniform4fv(loc, 1, reinterpret_cast<const float*>(bytes.data())); break;  // vec4
                case 64: glUniformMatrix4fv(loc, 1, GL_FALSE, reinterpret_cast<const float*>(bytes.data())); break; // mat4
                default: break;
                }
            }
            // textures
            GLint texUnit = 0;
            for (const auto& [name, tex] : fb->GetTextures()) {
                const GLuint gltex = as_gl_uint(tex ? tex->GetNativeHandle() : nullptr);
                if (!gltex) continue;
                glActiveTexture(GL_TEXTURE0 + texUnit);
                glBindTexture(GL_TEXTURE_2D, gltex);
                if (GLint samplerLoc = glGetUniformLocation(prog, name.c_str()); samplerLoc >= 0)
                    glUniform1i(samplerLoc, texUnit);
                ++texUnit;
            }
        }

        // Draw instanced
        const GLsizei indexCount = static_cast<GLsizei>(glMesh->GetIndexCount());
        glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(instanceCount));

        // Cleanup instance attrib binding (optional)
        glVertexAttribDivisor(ATTR_LOCATION, 0);
        glDisableVertexAttribArray(ATTR_LOCATION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

} // namespace GLInstancedSubmit

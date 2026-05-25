#include "Interstellar/Renderers/OpenGL/GLPointSubmit.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"

#include <glad/glad.h>
#include <cstdint>
#include <memory>

#include "Interstellar/Graphics/IRenderPipeline.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Graphics/IMaterial.hpp" // includes FallbackMaterial

namespace Interstellar::Renderers::OpenGL {

    using namespace Interstellar::Graphics;

    namespace {

        // Upload uniforms & textures from our concrete material
        inline void PushMaterialToGL(GLuint prog, const std::shared_ptr<IMaterial>& mat)
        {
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

        // ----------- Shared buffers (separate for 2D / 3D) -----------
        static GLuint sVAO2D = 0, sVBO2D = 0;
        inline void EnsureBuffers2D()
        {
            if (!sVAO2D) glGenVertexArrays(1, &sVAO2D);
            if (!sVBO2D) glGenBuffers(1, &sVBO2D);
        }

        static GLuint sVAO3D = 0, sVBO3D = 0;
        inline void EnsureBuffers3D()
        {
            if (!sVAO3D) glGenVertexArrays(1, &sVAO3D);
            if (!sVBO3D) glGenBuffers(1, &sVBO3D);
        }

        static GLuint sVAO3D_XYZR = 0, sVBO3D_XYZR = 0;
        inline void EnsureBuffers3D_XYZR()
        {
            if (!sVAO3D_XYZR) glGenVertexArrays(1, &sVAO3D_XYZR);
            if (!sVBO3D_XYZR) glGenBuffers(1, &sVBO3D_XYZR);
        }

        // XYZRI: position (vec3) + radius (float) + intensity (float), stride = 20 bytes
        static GLuint sVAO3D_XYZRI = 0, sVBO3D_XYZRI = 0;
        inline void EnsureBuffers3D_XYZRI()
        {
            if (!sVAO3D_XYZRI) glGenVertexArrays(1, &sVAO3D_XYZRI);
            if (!sVBO3D_XYZRI) glGenBuffers(1, &sVBO3D_XYZRI);
        }

        // Fullscreen quad: two triangles covering NDC [-1,1]^2
        static GLuint sVAO_FSQUAD = 0, sVBO_FSQUAD = 0;
        inline void EnsureBuffersFSQuad()
        {
            if (sVAO_FSQUAD) return;
            static constexpr float verts[] = {
                -1.f, -1.f,   1.f, -1.f,   1.f,  1.f,
                -1.f, -1.f,   1.f,  1.f,  -1.f,  1.f
            };
            glGenVertexArrays(1, &sVAO_FSQUAD);
            glGenBuffers(1, &sVBO_FSQUAD);
            glBindVertexArray(sVAO_FSQUAD);
            glBindBuffer(GL_ARRAY_BUFFER, sVBO_FSQUAD);
            glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
            glBindVertexArray(0);
        }

    } // anonymous namespace

    // ================= 2D points: [x0,y0, x1,y1, ...] =================
    void GLPointSubmit::draw(IGraphics&,
        const std::shared_ptr<IRenderPipeline>& pipeline,
        const std::shared_ptr<IMaterial>& material,
        const float* xyPacked,
        int count)
    {
        if (!pipeline || !material || !xyPacked || count <= 0) return;

        auto shader = pipeline->GetShader();
        const GLuint prog = static_cast<GLuint>(
            reinterpret_cast<uintptr_t>(shader ? shader->GetNativeHandle() : nullptr));
        if (!prog) return;

        glUseProgram(prog);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Save & adjust depth state to avoid alpha sprites writing depth
        const GLboolean prevDepthTest = glIsEnabled(GL_DEPTH_TEST);
        GLboolean prevDepthMask;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        // Push uniforms/textures from material
        PushMaterialToGL(prog, material);

        // Upload point positions (packed XY pairs)
        EnsureBuffers2D();
        glBindVertexArray(sVAO2D);
        glBindBuffer(GL_ARRAY_BUFFER, sVBO2D);
        glBufferData(GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(sizeof(float) * 2 * count),
            xyPacked,
            GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0); // layout(location=0) in vec2 a_StarPos;
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<void*>(0));

        // Draw
        glDrawArrays(GL_POINTS, 0, count);

        glBindVertexArray(0);

        // Restore
        if (prevDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        glDepthMask(prevDepthMask);
    }

    // Convenience alias kept for tests/back-compat
    void GLPointSubmit::draw2D(IGraphics& gfx,
        const std::shared_ptr<IRenderPipeline>& pipeline,
        const std::shared_ptr<IMaterial>& material,
        const float* xyPacked,
        int count)
    {
        GLPointSubmit::draw(gfx, pipeline, material, xyPacked, count);
    }

    // ================= 3D points: [x0,y0,z0, x1,y1,z1, ...] =================
    void GLPointSubmit::draw3D(IGraphics& gfx,
        const std::shared_ptr<IRenderPipeline>& pipeline,
        const std::shared_ptr<IMaterial>& material,
        const float* positionsXYZ,
        int count)
    {
        if (auto* ogl = dynamic_cast<OpenGLGraphics*>(&gfx)) {
            draw3D(*ogl, pipeline, material, positionsXYZ, count);
        }
    }

    void GLPointSubmit::draw3D(OpenGLGraphics&,
        std::shared_ptr<IRenderPipeline> pipeline,
        std::shared_ptr<IMaterial> material,
        const float* positionsXYZ,
        int count)
    {
        if (!pipeline || !positionsXYZ || count <= 0) return;

        auto shader = pipeline->GetShader();
        if (!shader) return;
        GLuint prog = static_cast<GLuint>(reinterpret_cast<uintptr_t>(shader->GetNativeHandle()));
        if (!prog) return;

        EnsureBuffers3D();

        glBindVertexArray(sVAO3D);
        glBindBuffer(GL_ARRAY_BUFFER, sVBO3D);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * count, positionsXYZ, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

        glUseProgram(prog);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Push uniforms (e.g., u_VP, u_CamPos, sizes, etc.) for the 3D shader
        if (material) {
            PushMaterialToGL(prog, material);
        }

        // For star "sky" points we can ignore depth writes/tests
        const GLboolean prevDepthTest = glIsEnabled(GL_DEPTH_TEST);
        GLboolean prevDepthMask;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glDrawArrays(GL_POINTS, 0, count);

        // Restore depth state
        if (prevDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        glDepthMask(prevDepthMask);

        glBindVertexArray(0);
        // glUseProgram(0); // optional
    }

    // ================= 3D points with per-point radius: [x,y,z,r, ...] =================
    void GLPointSubmit::draw3D_xyzr(IGraphics& gfx,
        const std::shared_ptr<IRenderPipeline>& pipeline,
        const std::shared_ptr<IMaterial>& material,
        const float* positionsXYZR,
        int count)
    {
        if (auto* ogl = dynamic_cast<OpenGLGraphics*>(&gfx)) {
            draw3D_xyzr(*ogl, pipeline, material, positionsXYZR, count);
        }
    }

    void GLPointSubmit::draw3D_xyzr(OpenGLGraphics&,
        std::shared_ptr<IRenderPipeline> pipeline,
        std::shared_ptr<IMaterial> material,
        const float* positionsXYZR,
        int count)
    {
        if (!pipeline || !positionsXYZR || count <= 0) return;

        auto shader = pipeline->GetShader();
        if (!shader) return;
        GLuint prog = static_cast<GLuint>(reinterpret_cast<uintptr_t>(shader->GetNativeHandle()));
        if (!prog) return;

        EnsureBuffers3D_XYZR();

        glBindVertexArray(sVAO3D_XYZR);
        glBindBuffer(GL_ARRAY_BUFFER, sVBO3D_XYZR);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * count, positionsXYZR, GL_DYNAMIC_DRAW);

        // location 0 = a_Pos (vec3), location 1 = a_Radius (float)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(3 * sizeof(float)));

        glUseProgram(prog);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (material) PushMaterialToGL(prog, material);

        const GLboolean prevDepthTest = glIsEnabled(GL_DEPTH_TEST);
        GLboolean prevDepthMask;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glDrawArrays(GL_POINTS, 0, count);

        if (prevDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        glDepthMask(prevDepthMask);

        glBindVertexArray(0);
    }

    // ====== 3D XYZRI: per-point position + radius + intensity, additive blend ======
    void GLPointSubmit::draw3D_xyzri(IGraphics& gfx,
        const std::shared_ptr<IRenderPipeline>& pipeline,
        const std::shared_ptr<IMaterial>& material,
        const float* positionsXYZRI,
        int count)
    {
        if (!dynamic_cast<OpenGLGraphics*>(&gfx)) return;
        if (!pipeline || !positionsXYZRI || count <= 0) return;

        auto shader = pipeline->GetShader();
        if (!shader) return;
        GLuint prog = static_cast<GLuint>(reinterpret_cast<uintptr_t>(shader->GetNativeHandle()));
        if (!prog) return;

        EnsureBuffers3D_XYZRI();

        glBindVertexArray(sVAO3D_XYZRI);
        glBindBuffer(GL_ARRAY_BUFFER, sVBO3D_XYZRI);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * count, positionsXYZRI, GL_DYNAMIC_DRAW);

        // location 0 = a_Pos (vec3 @ offset 0), location 1 = a_Radius (float @ 12), location 2 = a_Intensity (float @ 16)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(4 * sizeof(float)));

        glUseProgram(prog);
        glEnable(GL_PROGRAM_POINT_SIZE);

        // Additive blending: star light accumulates over the background
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        if (material) PushMaterialToGL(prog, material);

        const GLboolean prevDepthTest = glIsEnabled(GL_DEPTH_TEST);
        GLboolean prevDepthMask;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glDrawArrays(GL_POINTS, 0, count);

        if (prevDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        glDepthMask(prevDepthMask);

        // Restore default blend so subsequent passes are unaffected
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(0);
    }

    // ====== Fullscreen quad: two triangles covering NDC [-1,1]^2, normal blend ======
    void GLPointSubmit::drawFullscreenQuad(IGraphics&,
        const std::shared_ptr<IRenderPipeline>& pipeline,
        const std::shared_ptr<IMaterial>& material)
    {
        if (!pipeline || !material) return;
        auto shader = pipeline->GetShader();
        const GLuint prog = static_cast<GLuint>(
            reinterpret_cast<uintptr_t>(shader ? shader->GetNativeHandle() : nullptr));
        if (!prog) return;

        glUseProgram(prog);
        PushMaterialToGL(prog, material);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        const GLboolean prevDepthTest = glIsEnabled(GL_DEPTH_TEST);
        GLboolean prevDepthMask;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        EnsureBuffersFSQuad();
        glBindVertexArray(sVAO_FSQUAD);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        if (prevDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        glDepthMask(prevDepthMask);
    }

} // namespace Interstellar::Renderers::OpenGL

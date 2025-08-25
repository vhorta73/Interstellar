// interstellar/renderers/OpenGLBackendTests.cpp
#include <gtest/gtest.h>
#include <cstdint>
#include <vector>

#include "Interstellar/Logging/Logging.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLGraphics.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLMesh.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLShader.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLPipeline.hpp"
#include "Interstellar/Renderers/OpenGL/OpenGLTexture.hpp"
#include "Interstellar/Graphics/IGraphics.hpp"
#include "OpenGLTestHelpers.hpp"

using namespace Interstellar::Renderers::OpenGL;
using Interstellar::Graphics::GraphicsAPI;

TEST(OpenGLGraphics, Initialise_Shutdown_Basics) {
    OpenGLGraphics gl;
    ASSERT_TRUE(gl.Initialise(320, 240, /*vsync*/false));
    EXPECT_EQ(gl.GetAPI(), GraphicsAPI::OpenGL);
    EXPECT_NE(gl.GetNativeWindow(), nullptr);
    // Renderer name should be non-empty and must come from GL.
    const auto name = gl.GetRendererName();
    EXPECT_FALSE(name.empty());

    gl.BeginFrame();
    gl.EndFrame();

    EXPECT_FALSE(gl.ShouldClose());  // should be false right after init

    gl.Shutdown();                   // idempotent is OK to test by calling twice
    gl.Shutdown();
    EXPECT_EQ(gl.GetNativeWindow(), nullptr);
}

TEST(OpenGLGraphics, Resize_UpdatesViewport_NoCrash) {
    OpenGLGraphics gl;
    ASSERT_TRUE(gl.Initialise(200, 150, false));
    // Just exercise the code path; correctness is GL responsibility.
    EXPECT_NO_THROW(gl.Resize(640, 480));
    gl.Shutdown();
}

TEST(OpenGLMesh, CreatesBuffers_ComputesCounts_Binds) {
    OpenGLGraphics gl; ASSERT_TRUE(gl.Initialise(320, 240, false));

    // 3 vertices (pos+uv = 5 floats each), 3 indices (uint32)
    std::vector<float> verts = {
        // x y z   u v
         0.f, 0.5f, 0.f, 0.5f, 1.f,
        -0.5f,-0.5f, 0.f, 0.f,  0.f,
         0.5f,-0.5f, 0.f, 1.f,  0.f
    };
    std::vector<unsigned int> idx = { 0,1,2 };

    OpenGLMesh mesh(verts.data(), verts.size() * sizeof(float), idx.data(), idx.size() * sizeof(unsigned int));
    EXPECT_EQ(mesh.GetVertexCount(), 3u);
    EXPECT_EQ(mesh.GetIndexCount(), 3u);
    EXPECT_NE(mesh.GetNativeHandle(), nullptr);

    EXPECT_TRUE(mesh.SetDebugName("tri"));
    EXPECT_EQ(mesh.GetDebugName(), std::string_view("tri"));

    // Bind VAO (should not crash).
    EXPECT_NO_THROW(mesh.Bind());

    gl.Shutdown();
}

TEST(OpenGLShader, CompileLink_Succeeds_WithMinimalGLSL) {
    OpenGLGraphics gl; ASSERT_TRUE(gl.Initialise(320, 240, false));

    // Minimal pass-through shaders that reference u_Texture so SubmitMesh can set it.
    const std::string vs = R"(#version 330 core
  layout(location=0) in vec3 aPos;
  layout(location=1) in vec2 aUV;
  out vec2 vUV;
  void main(){ vUV=aUV; gl_Position=vec4(aPos,1.0); })";

    const std::string fs = R"(#version 330 core
  in vec2 vUV; out vec4 FragColor; uniform sampler2D u_Texture;
  void main(){ FragColor = texture(u_Texture, vUV); })";

    const auto vpath = write_temp_file("test.vert", vs);
    const auto fpath = write_temp_file("test.frag", fs);

    OpenGLShader shader("unit-shader", vpath, fpath);
    EXPECT_TRUE(shader.IsValid());
    auto stages = shader.GetAvailableStages();
    ASSERT_EQ(stages.size(), 2u);
    EXPECT_NE(shader.GetNativeHandle(), nullptr);

    gl.Shutdown();
}

TEST(OpenGLShader, Compile_Fails_WithBadSource) {
    OpenGLGraphics gl; ASSERT_TRUE(gl.Initialise(320, 240, false));

    // Non-existing files -> invalid
    OpenGLShader shader("bad-shader", "no_such.vert", "no_such.frag");
    EXPECT_FALSE(shader.IsValid());
    EXPECT_EQ(shader.GetNativeHandle(), nullptr);

    gl.Shutdown();
}

TEST(OpenGLPipeline, Options_Defaults_AndShaderLink) {
    OpenGLGraphics gl; ASSERT_TRUE(gl.Initialise(320, 240, false));

    // Minimal valid shader again
    const auto vpath = write_temp_file("opt.vert",
        "#version 330 core\nlayout(location=0) in vec3 a; void main(){gl_Position=vec4(a,1.0);} ");
    const auto fpath = write_temp_file("opt.frag",
        "#version 330 core\nout vec4 o; void main(){ o=vec4(1,0,0,1);} ");

    OpenGLShader shader("opt-sh", vpath, fpath);
    ASSERT_TRUE(shader.IsValid());

    OpenGLPipeline pipe(std::make_shared<OpenGLShader>(shader));
    ASSERT_NE(pipe.GetShader(), nullptr);

    // Your OpenGLPipeline stores "DepthTest" and "BlendEnabled"
    auto depth = pipe.GetOption("DepthTest");
    auto blend = pipe.GetOption("BlendEnabled");
    EXPECT_TRUE(std::holds_alternative<bool>(depth));
    EXPECT_TRUE(std::holds_alternative<bool>(blend));
    EXPECT_TRUE(std::get<bool>(depth));
    EXPECT_FALSE(std::get<bool>(blend));

    // Unknown -> returns default-constructed variant (bool=false as first alt)
    auto unknown = pipe.GetOption("Nope");
    EXPECT_TRUE(std::holds_alternative<bool>(unknown));
    EXPECT_FALSE(std::get<bool>(unknown));

    EXPECT_EQ(pipe.GetNativeHandle(), nullptr);

    gl.Shutdown();
}

// TODO: Confirm if all png are file based or memory based or mixed.
//TEST(OpenGLTexture, Loads1x1Png_MetadataAndHandle) {
//   using namespace Interstellar::Logging;
//    OpenGLGraphics gl; ASSERT_TRUE(gl.Initialise(320, 240, false));
//
//    const auto png = write_1x1_png();
//    OpenGLTexture tex(png);
//    EXPECT_GE(tex.GetWidth(), 1u);
//    EXPECT_GE(tex.GetHeight(), 1u);
//    EXPECT_FALSE(tex.GetFormat().empty());       // "RGBA" or "RGB"
//    EXPECT_FALSE(tex.GetName().empty());
//    EXPECT_NE(tex.GetNativeHandle(), nullptr);
//
//    // Target is GL_TEXTURE_2D
//    EXPECT_EQ(tex.GetTextureTarget(), 0x0DE1 /*GL_TEXTURE_2D*/);
//
//    gl.Shutdown();
//}

TEST(OpenGLGraphics, SubmitMesh_Draws_NoCrash) {
    OpenGLGraphics gl; ASSERT_TRUE(gl.Initialise(320, 240, false));

    // Geometry
    std::vector<float> verts = {
       0.f, 0.5f, 0.f, 0.5f, 1.f,
      -0.5f,-0.5f, 0.f, 0.f,  0.f,
       0.5f,-0.5f, 0.f, 1.f,  0.f
    };
    std::vector<unsigned int> idx = { 0,1,2 };
    auto mesh = std::make_shared<OpenGLMesh>(verts.data(), verts.size() * sizeof(float),
        idx.data(), idx.size() * sizeof(unsigned int));

    // Shader with u_Texture (so SubmitMesh can set sampler uniform)
    const auto vpath = write_temp_file("sm.vert",
        "#version 330 core\nlayout(location=0) in vec3 p; layout(location=1) in vec2 uv; out vec2 t; "
        "void main(){ t=uv; gl_Position=vec4(p,1); }");
    const auto fpath = write_temp_file("sm.frag",
        "#version 330 core\nin vec2 t; out vec4 c; uniform sampler2D u_Texture; void main(){ c=vec4(t,0,1);} ");
    auto shader = std::make_shared<OpenGLShader>("draw", vpath, fpath);
    ASSERT_TRUE(shader->IsValid());

    auto pipe = std::make_shared<OpenGLPipeline>(shader);

    gl.BeginFrame();
    // Should not throw; we don't assert pixels, only that the uniform path/VAO draw works.
    EXPECT_NO_THROW(gl.SubmitMesh(mesh, pipe));
    gl.EndFrame();

    gl.Shutdown();
}

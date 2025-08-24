// tests/GraphicsInterfaceTests.cpp
#include <gtest/gtest.h>
#include "interstellar/graphics/FakeGraphics.hpp"

using namespace Interstellar::Graphics;
using namespace interstellar::graphics;

TEST(GraphicsIface, Lifecycle_InitBeginEndShutdown) {
    FakeGraphics g;
    EXPECT_FALSE(g.initialised);
    EXPECT_TRUE(g.Initialise(800, 600, true));
    EXPECT_TRUE(g.initialised);
    EXPECT_EQ(g.w, 800u); EXPECT_EQ(g.h, 600u);
    EXPECT_TRUE(g.vsync);
    EXPECT_NE(g.GetNativeWindow(), nullptr);
    EXPECT_EQ(g.GetAPI(), GraphicsAPI::OpenGL);
    EXPECT_FALSE(g.ShouldClose());
    EXPECT_NO_THROW(g.BeginFrame());
    EXPECT_THROW(g.BeginFrame(), std::logic_error);   // double begin
    EXPECT_NO_THROW(g.EndFrame());
    EXPECT_THROW(g.EndFrame(), std::logic_error);     // double end
    g.Shutdown();
    EXPECT_FALSE(g.initialised);
    EXPECT_EQ(g.GetNativeWindow(), nullptr);
}

TEST(GraphicsIface, Resize_AfterInit_UpdatesDimensions) {
    FakeGraphics g; ASSERT_TRUE(g.Initialise(1280, 720, false));
    EXPECT_NO_THROW(g.Resize(1920, 1080));
    EXPECT_EQ(g.w, 1920u); EXPECT_EQ(g.h, 1080u);
}

TEST(GraphicsIface, SubmitMesh_RequiresFrame_And_NonNull) {
    FakeGraphics g; ASSERT_TRUE(g.Initialise(640, 480, false));
    auto mesh = g.CreateMesh("abc", 3, "xy", 2);
    auto sh = g.CreateShader("basic");
    auto pipe = g.CreatePipeline(sh);
    EXPECT_THROW(g.SubmitMesh(mesh, pipe), std::logic_error);  // not in frame
    g.BeginFrame();
    EXPECT_THROW(g.SubmitMesh(nullptr, pipe), std::invalid_argument);
    EXPECT_THROW(g.SubmitMesh(mesh, nullptr), std::invalid_argument);
    EXPECT_NO_THROW(g.SubmitMesh(mesh, pipe));
    g.EndFrame();
    EXPECT_EQ(g.submitCount, 1);
}

TEST(GraphicsIface, CreateMesh_CopiesInputBuffers_AndCounts) {
    FakeGraphics g; ASSERT_TRUE(g.Initialise(1, 1, false));
    std::vector<std::byte> v(4), i(2);
    v[0] = std::byte{ 1 }; i[0] = std::byte{ 9 };
    auto m = g.CreateMesh(v.data(), v.size(), i.data(), i.size());
    ASSERT_NE(m, nullptr);
    // mutate sources; mesh must retain its own copies
    v[0] = std::byte{ 0 }; i[0] = std::byte{ 0 };
    EXPECT_EQ(m->GetVertexCount(), v.size());
    EXPECT_EQ(m->GetIndexCount(), i.size());
    auto* fm = dynamic_cast<FakeMesh*>(m.get());
    ASSERT_NE(fm, nullptr);
    EXPECT_EQ(std::to_integer<int>(fm->vbuf[0]), 1);
    EXPECT_EQ(std::to_integer<int>(fm->ibuf[0]), 9);
    EXPECT_NE(m->GetNativeHandle(), nullptr);

    // debug name
    EXPECT_TRUE(m->SetDebugName("ship"));
    EXPECT_EQ(m->GetDebugName(), std::string_view("ship"));
}

TEST(GraphicsIface, Shader_Basics) {
    FakeGraphics g; ASSERT_TRUE(g.Initialise(1, 1, false));
    auto s = g.CreateShader("phong");
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->GetName(), "phong");
    auto stages = s->GetAvailableStages();
    ASSERT_EQ(stages.size(), 2u);
    EXPECT_TRUE(s->IsValid());
    EXPECT_NE(s->GetNativeHandle(), nullptr);
}

TEST(GraphicsIface, Pipeline_Options_And_ShaderLink) {
    FakeGraphics g; ASSERT_TRUE(g.Initialise(1, 1, false));
    auto s = g.CreateShader("basic");
    auto p = g.CreatePipeline(s);
    ASSERT_NE(p, nullptr);
    ASSERT_NE(p->GetShader(), nullptr);
    // Known options & types
    EXPECT_TRUE(std::get<bool>(p->GetOption("depthTest")));
    EXPECT_EQ(std::get<int>(p->GetOption("cullMode")), 2);
    EXPECT_EQ(std::get<float>(p->GetOption("lineWidth")), 1.0f);
    EXPECT_EQ(std::get<std::string>(p->GetOption("label")), "Default");
    // Unknown throws
    EXPECT_THROW((void)p->GetOption("nope"), std::out_of_range);
    EXPECT_NE(p->GetNativeHandle(), nullptr);
}

TEST(GraphicsIface, Texture_Basics) {
    FakeGraphics g; ASSERT_TRUE(g.Initialise(1, 1, false));
    auto t = g.CreateTexture("albedo.png");
    ASSERT_NE(t, nullptr);
    EXPECT_EQ(t->GetName(), "albedo.png");
    EXPECT_GT(t->GetWidth(), 0u);
    EXPECT_GT(t->GetHeight(), 0u);
    EXPECT_FALSE(t->GetFormat().empty());
    EXPECT_NE(t->GetNativeHandle(), nullptr);
}

TEST(GraphicsIface, MultiFrame_SubmitCounts) {
    FakeGraphics g; ASSERT_TRUE(g.Initialise(1, 1, false));
    auto m = g.CreateMesh(nullptr, 0, nullptr, 0);
    auto p = g.CreatePipeline(g.CreateShader("s"));
    for (int i = 0; i < 3; ++i) {
        g.BeginFrame();
        g.SubmitMesh(m, p);
        g.EndFrame();
    }
    EXPECT_EQ(g.beginCount, 3);
    EXPECT_EQ(g.endCount, 3);
    EXPECT_EQ(g.submitCount, 3);
}

#pragma once

#include "Interstellar/Graphics/Core/IGraphics.hpp"

struct GLFWwindow; // Forward declare to avoid including GLFW in header.

namespace Interstellar::Graphics::OpenGL {

    class OpenGLGraphics : public Core::IGraphics {
    public:
        explicit OpenGLGraphics();
        ~OpenGLGraphics() override;

        bool Initialize(uint32_t width, uint32_t height, bool vsync) override;
        void Shutdown() override;

        void BeginFrame() override;
        void EndFrame() override;
        void Resize(uint32_t width, uint32_t height) override;

        [[nodiscard]] std::shared_ptr<Core::IMesh> CreateMesh(
            const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize
        ) override;

        [[nodiscard]] std::shared_ptr<Core::ITexture> CreateTexture(const std::string& path) override;
        [[nodiscard]] std::shared_ptr<Core::IShader> CreateShader(const std::string& name) override;
        [[nodiscard]] std::shared_ptr<Core::IRenderPipeline> CreatePipeline(std::shared_ptr<Core::IShader> shader) override;

        void SubmitMesh(
            std::shared_ptr<Core::IMesh> mesh,
            std::shared_ptr<Core::IRenderPipeline> pipeline
        ) override;

        [[nodiscard]] std::string GetRendererName() const override;
        [[nodiscard]] Core::GraphicsAPI GetAPI() const override;
        [[nodiscard]] bool ShouldClose() const override;

    private:
        GLFWwindow* m_Window = nullptr;
        bool m_Vsync = true;
    };

}

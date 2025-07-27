#pragma once

#include "Interstellar/Graphics/Core/IGraphics.hpp"
#include <GLFW/glfw3.h>
#include <memory>

namespace Interstellar::Graphics::OpenGL {

    class OpenGLGraphics : public Core::IGraphics {
    public:
        OpenGLGraphics();
        ~OpenGLGraphics();

        bool Initialize(void* windowHandle, uint32_t width, uint32_t height, bool vsync) override;
        void Shutdown() override;

        void BeginFrame() override;
        void EndFrame() override;

        void Resize(uint32_t width, uint32_t height) override;

        std::shared_ptr<Core::IMesh> CreateMesh(const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize) override;

        std::shared_ptr<Core::ITexture> CreateTexture(const std::string& path) override;
        std::shared_ptr<Core::IShader> CreateShader(const std::string& path) override;
        std::shared_ptr<Core::IRenderPipeline> CreatePipeline(std::shared_ptr<Core::IShader> shader) override;

        void SubmitMesh(std::shared_ptr<Core::IMesh> mesh, std::shared_ptr<Core::IRenderPipeline> pipeline) override;

        std::string GetRendererName() const override;
        Core::GraphicsAPI GetAPI() const override;

    private:
        GLFWwindow* m_Window = nullptr;
        bool m_Vsync = true;
    };

}

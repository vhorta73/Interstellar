#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Interstellar::Graphics::Core {

    class IMesh;
    class ITexture;
    class IShader;
    class IRenderPipeline;

    enum class GraphicsAPI {
        Vulkan,
        DirectX12,
        OpenGL,
        Metal,
        Unknown
    };

    class IGraphics {
    public:
        virtual ~IGraphics() = default;

        virtual bool Initialize(uint32_t width, uint32_t height, bool vsync) = 0;
        virtual void Shutdown() = 0;
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual std::shared_ptr<IMesh> CreateMesh(const void* vertexData, size_t vertexSize,
            const void* indexData, size_t indexSize) = 0;
        virtual std::shared_ptr<ITexture> CreateTexture(const std::string& path) = 0;
        virtual std::shared_ptr<IShader> CreateShader(const std::string& path) = 0;
        virtual std::shared_ptr<IRenderPipeline> CreatePipeline(std::shared_ptr<IShader> shader) = 0;
        virtual void SubmitMesh(std::shared_ptr<IMesh> mesh, std::shared_ptr<IRenderPipeline> pipeline) = 0;
        virtual std::string GetRendererName() const = 0;
        virtual GraphicsAPI GetAPI() const = 0;
        virtual bool ShouldClose() const = 0;
    };
}

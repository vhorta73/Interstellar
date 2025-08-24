// interstellar/graphics/FakeGraphics.hpp
#pragma once
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <cstddef>   // std::byte

#include "Interstellar/Graphics/IGraphics.hpp"
#include "Interstellar/Graphics/IMesh.hpp"
#include "Interstellar/Graphics/ITexture.hpp"
#include "Interstellar/Graphics/IShader.hpp"
#include "Interstellar/Graphics/IRenderPipeline.hpp"

// short alias so we don't repeat the long qualified name everywhere
namespace ig = ::Interstellar::Graphics;

namespace interstellar::graphics {

    struct FakeMesh final : ig::IMesh {
        std::string name;
        std::vector<std::byte> vbuf, ibuf;
        void* native{ reinterpret_cast<void*>(static_cast<uintptr_t>(0xDEADBEEF)) };

        std::string_view GetDebugName() const override { return name; }
        virtual bool SetDebugName(std::string_view n) { name.assign(n.begin(), n.end()); return true; }
        size_t GetVertexCount() const override { return vbuf.size(); }
        size_t GetIndexCount()  const override { return ibuf.size(); }
        void* GetNativeHandle() const override { return native; }
    };

    struct FakeShader final : ig::IShader {
        std::string name;
        std::vector<std::string> stages;
        bool valid{ true };
        void* native{ reinterpret_cast<void*>(static_cast<uintptr_t>(0xABCD1234)) };

        const std::string& GetName() const override { return name; }
        std::vector<std::string> GetAvailableStages() const override { return stages; }
        bool IsValid() const override { return valid; }
        void* GetNativeHandle() const override { return native; }
    };

    struct FakePipeline final : ig::IRenderPipeline {
        std::shared_ptr<ig::IShader> shader;
        std::unordered_map<std::string, ig::PipelineOptionValue> opts;
        void* native{ reinterpret_cast<void*>(static_cast<uintptr_t>(0xFACEB00C)) };

        std::shared_ptr<ig::IShader> GetShader() const override { return shader; }
        ig::PipelineOptionValue GetOption(const std::string& name) const override {
            auto it = opts.find(name);
            if (it == opts.end()) throw std::out_of_range("unknown option");
            return it->second;
        }
        void* GetNativeHandle() const override { return native; }
    };

    struct FakeGraphics final : ig::IGraphics {
        ig::GraphicsAPI api{ ig::GraphicsAPI::OpenGL };
        std::string renderer{ "FakeGL 1.0" };
        bool initialised{ false };
        bool vsync{ false };
        bool shouldClose{ false };
        uint32_t w{ 0 }, h{ 0 };
        void* window{ nullptr };

        // frame state
        bool inFrame{ false };
        int beginCount{ 0 }, endCount{ 0 }, submitCount{ 0 };

        bool Initialise(uint32_t width, uint32_t height, bool vs) override {
            if (initialised) return false;
            initialised = true; w = width; h = height; vsync = vs;
            window = reinterpret_cast<void*>(static_cast<uintptr_t>(0xCAFEBABE));
            return true;
        }
        void Shutdown() override {
            inFrame = false;
            initialised = false;
            window = nullptr;
        }

        void BeginFrame() override {
            if (!initialised) throw std::logic_error("BeginFrame without init");
            if (inFrame) throw std::logic_error("BeginFrame twice");
            inFrame = true; ++beginCount;
        }
        void EndFrame() override {
            if (!inFrame) throw std::logic_error("EndFrame without BeginFrame");
            inFrame = false; ++endCount;
        }

        void Resize(uint32_t width, uint32_t height) override {
            if (!initialised) throw std::logic_error("Resize without init");
            w = width; h = height;
        }

        std::shared_ptr<ig::IMesh>
            CreateMesh(const void* vertexData, size_t vertexSize,
                const void* indexData, size_t indexSize) override {
            auto m = std::make_shared<FakeMesh>();
            if (vertexSize) {
                if (!vertexData) return {};
                m->vbuf.resize(vertexSize);
                std::memcpy(m->vbuf.data(), vertexData, vertexSize);
            }
            if (indexSize) {
                if (!indexData) return {};
                m->ibuf.resize(indexSize);
                std::memcpy(m->ibuf.data(), indexData, indexSize);
            }
            return m;
        }

        std::shared_ptr<ig::ITexture> CreateTexture(const std::string& path) override {
            struct T final : ig::ITexture {
                uint32_t w{ 64 }, h{ 64 }; std::string fmt{ "RGBA8" }; std::string name; void* native{ reinterpret_cast<void*>(0x1111) };
                uint32_t GetWidth()  const override { return w; }
                uint32_t GetHeight() const override { return h; }
                std::string GetFormat() const override { return fmt; }
                const std::string& GetName() const override { return name; }
                void* GetNativeHandle() const override { return native; }
            };
            auto t = std::make_shared<T>();
            t->name = path;
            return t;
        }

        std::shared_ptr<ig::IShader> CreateShader(const std::string& path) override {
            auto s = std::make_shared<FakeShader>();
            s->name = path;
            s->stages = { "vertex","fragment" };
            s->valid = true;
            return s;
        }

        std::shared_ptr<ig::IRenderPipeline> CreatePipeline(std::shared_ptr<ig::IShader> shader) override {
            if (!shader) return {};
            auto p = std::make_shared<FakePipeline>();
            p->shader = shader;
            p->opts["depthTest"] = true;
            p->opts["cullMode"] = 2;      // e.g., back
            p->opts["alpha"] = false;
            p->opts["lineWidth"] = 1.0f;
            p->opts["label"] = std::string("Default");
            return p;
        }

        void SubmitMesh(std::shared_ptr<ig::IMesh> mesh,
            std::shared_ptr<ig::IRenderPipeline> pipeline) override {
            if (!inFrame) throw std::logic_error("Submit outside Begin/EndFrame");
            if (!mesh || !pipeline) throw std::invalid_argument("null args");
            ++submitCount;
        }

        std::string GetRendererName() const override { return renderer; }
        ig::GraphicsAPI GetAPI() const override { return api; }
        bool ShouldClose() const override { return shouldClose; }
        void* GetNativeWindow() const override { return window; }
    };

} // namespace interstellar::graphics

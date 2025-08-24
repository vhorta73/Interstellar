#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Interstellar::Graphics {

    class ITexture;

    /**
     * Minimal per-draw binding object. Backends can ignore it until implemented.
     * This header also ships a generic storage-only fallback so nothing breaks.
     */
    class IMaterial {
    public:
        virtual ~IMaterial() = default;

        // Name-based small constant blob (push-constant/uniform equivalent)
        virtual bool Set(const std::string& /*name*/, const void* /*data*/, std::size_t /*bytes*/) { return false; }

        // Name-based texture binding
        virtual bool Set(const std::string& /*name*/, std::shared_ptr<ITexture> /*tex*/) { return false; }

        // Explicit set/binding for descriptor-style APIs (ignored by old backends)
        virtual bool Set(uint32_t /*setIndex*/, uint32_t /*bindingIndex*/, std::shared_ptr<ITexture> /*tex*/) { return false; }

        virtual bool SetPushConstants(const void* /*data*/, std::size_t /*bytes*/, uint32_t /*stageMask*/ = 0xFFFFFFFFu) { return false; }

        virtual void Clear() {}
    };

    /* ---------- Generic fallback material (storage only) ---------- */
    class FallbackMaterial final : public IMaterial {
    public:
        bool Set(const std::string& name, const void* data, std::size_t bytes) override {
            auto& buf = constants[name];
            buf.assign(reinterpret_cast<const std::byte*>(data), reinterpret_cast<const std::byte*>(data) + bytes);
            return true;
        }
        bool Set(const std::string& name, std::shared_ptr<ITexture> tex) override {
            textures[name] = std::move(tex);
            return true;
        }
        bool Set(uint32_t setIndex, uint32_t bindingIndex, std::shared_ptr<ITexture> tex) override {
            explicitTextures[{setIndex, bindingIndex}] = std::move(tex);
            return true;
        }
        bool SetPushConstants(const void* data, std::size_t bytes, uint32_t) override {
            pc.assign(reinterpret_cast<const std::byte*>(data), reinterpret_cast<const std::byte*>(data) + bytes);
            return true;
        }
        void Clear() override { constants.clear(); textures.clear(); explicitTextures.clear(); pc.clear(); }

        // Accessors for backends that want to read the stored data:
        const auto& GetConstants() const { return constants; }
        const auto& GetTextures()  const { return textures; }
        const auto& GetExplicit()  const { return explicitTextures; }
        const auto& GetPushConst() const { return pc; }

    private:
        std::unordered_map<std::string, std::vector<std::byte>> constants;
        std::unordered_map<std::string, std::shared_ptr<ITexture>> textures;
        struct SB { uint32_t set, binding; bool operator==(const SB& o) const { return set == o.set && binding == o.binding; } };
        struct SBHash { size_t operator()(const SB& k) const { return (size_t(k.set) << 32) ^ size_t(k.binding); } };
        std::unordered_map<SB, std::shared_ptr<ITexture>, SBHash> explicitTextures;
        std::vector<std::byte> pc;
    };

} // namespace Interstellar::Graphics

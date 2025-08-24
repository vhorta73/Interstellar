// Interstellar/Graphics/MaterialHelpers.hpp
#pragma once
#include "Interstellar/Graphics/IMaterial.hpp"
#include <glm/glm.hpp>

namespace Interstellar::Graphics {
    inline bool SetFloat(IMaterial& m, const char* name, float v) { return m.Set(name, &v, sizeof(v)); }
    inline bool SetVec2(IMaterial& m, const char* name, const glm::vec2& v) { return m.Set(name, &v, sizeof(v)); }
    inline bool SetMat4(IMaterial& m, const char* name, const glm::mat4& v) { return m.Set(name, &v, sizeof(v)); }
} // namespace

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Interstellar::Engine {

    struct Camera3D {
        glm::vec3 pos{ 0.0f, 0.0f, 6.0f };  // start a bit "away"
        float yaw = 0.0f;               // radians, 0 = looking toward -Z when pitch=0 (we'll keep forward fixed if you want)
        float pitch = 0.0f;               // radians
        float fovY_deg = 60.0f;           // vertical FOV
        float aspect = 1.0f;
        float nearZ = 0.05f;
        float farZ = 300.0f;

        // Simple FPS-style basis (right-handed, +Y up)
        glm::vec3 forward() const {
            // keep it simple: forward fixed to (0,0,-1) unless you rotate with mouse
            float cp = cosf(pitch), sp = sinf(pitch);
            float cy = cosf(yaw), sy = sinf(yaw);
            // Yaw around +Y, then pitch around +X:
            return glm::normalize(glm::vec3(sy * cp, sp, -cy * cp));
        }
        glm::vec3 right()   const { return glm::normalize(glm::cross(forward(), glm::vec3(0, 1, 0))); }
        glm::vec3 up()      const { return glm::normalize(glm::cross(right(), forward())); }

        glm::mat4 view() const {
            return glm::lookAt(pos, pos + forward(), up());
        }
        glm::mat4 proj() const {
            return glm::perspective(glm::radians(fovY_deg), aspect, nearZ, farZ);
        }
        glm::mat4 vp() const { return proj() * view(); }

        void setAspect(int w, int h) { aspect = (h > 0) ? float(w) / float(h) : 1.0f; }
        void clampPitch() {
            const float limit = glm::radians(89.0f);
            if (pitch > limit) pitch = limit;
            if (pitch < -limit) pitch = -limit;
        }
    };

} // namespace Interstellar::Engine

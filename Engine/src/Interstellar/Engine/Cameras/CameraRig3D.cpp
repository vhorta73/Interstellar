#include "Interstellar/Engine/Cameras/CameraRig3D.hpp"

namespace Interstellar::Engine::Cameras {
    using namespace Interstellar::Input;

    static inline float exp_smooth(float current, float target, float halflife, double dt) {
        if (halflife <= 0.0f) return target;
        const float k = std::exp2f(static_cast<float>(-dt) / halflife); // 2^(-dt/hl)
        return target + (current - target) * k;
    }

    static inline glm::vec2 exp_smooth(glm::vec2 c, glm::vec2 t, float hl, double dt) {
        return { exp_smooth(c.x, t.x, hl, dt), exp_smooth(c.y, t.y, hl, dt) };
    }

    // Use WheelZoom multiplicatively; invert for FOV (wheel up -> zoom in = smaller FOV).
    static inline float ApplyWheelAsFovZoom(const IMouse& mouse, float currentFov,
        float speed, float minFov, float maxFov) {
        float factor = WheelZoom(mouse, 1.0f, speed, 0.25f, 4.0f); // >1 on wheel up
        if (std::abs(factor - 1.0f) > 1e-6f) {
            currentFov = currentFov / factor;
            currentFov = std::clamp(currentFov, minFov, maxFov);
        }
        return currentFov;
    }

    void CameraRig3D::handleInput(const IKeyboard& kb, const IMouse& mouse,
        int viewportW, int viewportH, double dt)
    {
        // Arrow keys pan XY (act on targets)
        float pan = params_.panBase * static_cast<float>(dt);
        if (kb.isDown(KeyCode::ArrowLeft))  targetCenter.x += pan;
        if (kb.isDown(KeyCode::ArrowRight)) targetCenter.x -= pan;
        if (kb.isDown(KeyCode::ArrowUp))    targetCenter.y -= pan;
        if (kb.isDown(KeyCode::ArrowDown))  targetCenter.y += pan;

        // Drag to pan (dragging the world)
        static MouseFilter drag{ 0.5f };
        if (mouse.isDown(MouseButton::Left)) {
            glm::vec2 d = drag.apply(mouse, viewportW, viewportH);
            targetCenter -= d;
        }
        else {
            drag.reset();
        }

        // Wheel:
        //  - Shift or 'Z' held => zoom (FOV)
        //  - Otherwise => Dolly along Z
        const bool zoomHeld =
            kb.isDown(KeyCode::LeftShift) || kb.isDown(KeyCode::RightShift) || kb.isDown(KeyCode::Z);

        if (zoomHeld) {
            targetFovDeg = ApplyWheelAsFovZoom(mouse, targetFovDeg, params_.dollySpeed,
                params_.minFov, params_.maxFov);
        }
        else {
            float newZ = WheelZoom(mouse, targetZ, params_.dollySpeed, params_.minZ, params_.maxZ);
            targetZ = std::clamp(newZ, params_.minZ, params_.maxZ);
        }
    }

    void CameraRig3D::update(double dt) {
        center = exp_smooth(center, targetCenter, params_.posHL, dt);
        z = exp_smooth(z, targetZ, params_.zHL, dt);
        fovDeg = exp_smooth(fovDeg, targetFovDeg, params_.fovHL, dt);
    }

    glm::mat4 CameraRig3D::view() const {
        const glm::vec3 eyeV(center.x, center.y, z);
        const glm::vec3 fwd(0, 0, -1);
        return glm::lookAt(eyeV, eyeV + fwd, glm::vec3(0, 1, 0));
    }

    glm::mat4 CameraRig3D::proj(float aspect) const {
        return glm::perspective(glm::radians(fovDeg), aspect, 0.1f, 1e8f);
    }

} // namespace Interstellar::Engine

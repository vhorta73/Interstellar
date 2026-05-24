#include "Interstellar/Engine/Cameras/CameraRig3D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <cmath>

#include <Interstellar/Input/IKeyboard.hpp>
#include <Interstellar/Input/IMouse.hpp>
#include <Interstellar/Input/MouseButtons.hpp>
#include <Interstellar/Input/KeyCodes.hpp>

namespace Interstellar::Engine::Cameras {

    using Interstellar::Input::KeyCode;

    struct CameraRig3D::DragImpl {
        bool  lmbPrev = false;
        float lastX = 0.0f, lastY = 0.0f;
    };
    
    void CameraRig3D::DragDeleter::operator()(DragImpl* p) noexcept { delete p; }

    CameraRig3D::~CameraRig3D() = default;

    glm::vec3 CameraRig3D::forward() const { return orient * glm::vec3(0, 0, -1); }
    glm::vec3 CameraRig3D::right() const { return orient * glm::vec3(1, 0, 0);  }
    glm::vec3 CameraRig3D::up() const { return orient * glm::vec3(0, 1, 0); }

    glm::mat4 CameraRig3D::view() const {
        glm::mat4 R = glm::toMat4(orient);
        glm::mat4 T = glm::translate(glm::mat4(1.0f), -pos);
        return R * T;
    }

    glm::mat4 CameraRig3D::proj(float aspect) const {
        // NOTE: must match your renderer’s far plane expectations.
        const float nearKm = 1.0f;     // 1 km
        const float farKm = 1.0e12f;  // 1e12 km
        return glm::perspective(glm::radians(fovDeg), std::max(0.001f, aspect), nearKm, farKm);
    }

    void CameraRig3D::setDragFocusDepth(float depthKm) { dragDepthKm_ = depthKm; }
    void CameraRig3D::setDragFocusDepth(std::optional<float> depthKm) { dragDepthKm_ = depthKm; }

    static inline float smoothTo(float cur, float target, float hl, float dt) {
        // exponential half-life smoothing
        if (hl <= 1e-6f) return target;
        float k = std::pow(0.5f, dt / std::max(1e-6f, hl));
        return target + (cur - target) * k;
    }

    void CameraRig3D::handleInput(const Interstellar::Input::IKeyboard& kb,
        Interstellar::Input::IMouse& mouse,
        int viewportW, int viewportH,
        double dt)
    {
        if (!drag_) drag_.reset(new DragImpl{});

        const bool spaceHeld = kb.isDown(KeyCode::Space);
        const bool zHeld = kb.isDown(KeyCode::Z);

        // Update yaw/pitch quaternion from angles
        auto clampPitch = [](float p) { return std::clamp(p, -89.9f, 89.9f); };
        pitchDeg = clampPitch(pitchDeg);
        orient = glm::angleAxis(glm::radians(yawDeg), glm::vec3(0, 1, 0))
            * glm::angleAxis(glm::radians(pitchDeg), glm::vec3(1, 0, 0));

        // Mouse
        const bool lmb = mouse.isDown(Interstellar::Input::MouseButton::Left);
        const auto  mpos = mouse.position();
        float mx = mpos.x, my = mpos.y;

        // Mouse wheel: dolly or FOV zoom (if 'Z' held)
        const float wheel = mouse.wheel().y;
        if (wheel != 0.0f) {
            if (zHeld) {
                // FOV zoom — clamp
                targetFovDeg = std::clamp(targetFovDeg - wheel * 2.0f, params_.minFov, params_.maxFov);
            }
            else {
                // Dolly: toward focus if present, else along forward
                float step = wheel * params_.dollyKmPerNotch;
                if (focusWorld_) {
                    glm::vec3 to = *focusWorld_ - pos;
                    float dist = glm::length(to);
                    glm::vec3 dir = (dist > 1e-6f) ? (to / dist) : forward();
                    // scale slightly with distance for reach; clamp to avoid overshoot
                    float scaled = step * (1.0f + 0.25f * std::sqrt(std::max(dist, 0.0f) / std::max(1.0f, params_.dollyKmPerNotch)));
                    if (scaled > 0.0f) scaled = std::min(scaled, std::max(0.0f, dist * 0.98f));
                    targetPos += dir * scaled;
                }
                else {
                    targetPos += forward() * step;
                }
            }
        }

        // Dragging behavior
        if (lmb && !drag_->lmbPrev) {
            drag_->lastX = mx; drag_->lastY = my;
            rotateActive_ = spaceHeld; // Space+LMB means "look around"
        }

        if (lmb) {
            float dx = mx - drag_->lastX;
            float dy = my - drag_->lastY;
            drag_->lastX = mx; drag_->lastY = my;

            if (spaceHeld) {
                // In-place look (no orbit): adjust yaw/pitch only.
                yawDeg += dx * params_.yawSensDegPerPx;
                pitchDeg += -dy * params_.pitchSensDegPerPx;
                pitchDeg = clampPitch(pitchDeg);
                orient = glm::angleAxis(glm::radians(yawDeg), glm::vec3(0, 1, 0))
                    * glm::angleAxis(glm::radians(pitchDeg), glm::vec3(1, 0, 0));
            }
            else {
                // 2D pan in camera plane, scaled by depth (if set)
                float depth = dragDepthKm_.value_or(1.0e9f);
                // pixels → NDC → world scale
                float sx = 2.0f * dx / std::max(1, viewportW);
                float sy = -2.0f * dy / std::max(1, viewportH);
                // scale to feel (dragMult), and depth to keep “screen-anchored” feel
                float k = params_.dragMult * depth * std::tan(glm::radians(fovDeg * 0.5f));
                // move right/up in world
                targetPos += right() * (sx * k) + up() * (sy * k);
            }
        }

        if (!lmb && drag_->lmbPrev) {
            rotateActive_ = false;
        }
        drag_->lmbPrev = lmb;

        float speed = params_.panBase;
        if (kb.isDown(KeyCode::LeftShift)) speed *= 4.0f;
        glm::vec3 move(0);
        if (kb.isDown(KeyCode::W)) move += forward();
        if (kb.isDown(KeyCode::S)) move -= forward();
        if (kb.isDown(KeyCode::A)) move -= right();
        if (kb.isDown(KeyCode::D)) move += right();
        if (kb.isDown(KeyCode::E)) move += up();
        if (kb.isDown(KeyCode::Q)) move -= up();
        if (glm::length2(move) > 0.0f) {
            targetPos += glm::normalize(move) * (speed * (float)dt);
        }

    }

    void CameraRig3D::update(double dt) {
        pos = glm::mix(targetPos, pos, std::pow(0.5f, static_cast<float>(dt) / std::max(1e-6f, params_.posHL)));
        fovDeg = smoothTo(fovDeg, targetFovDeg, params_.fovHL, static_cast<float>(dt));
    }

} // namespace Interstellar::Engine::Cameras

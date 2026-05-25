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
        bool  rmbPrev = false;
        float rLastX = 0.0f, rLastY = 0.0f;
    };

    void CameraRig3D::DragDeleter::operator()(DragImpl* p) noexcept { delete p; }

    CameraRig3D::~CameraRig3D() = default;

    glm::vec3 CameraRig3D::forward() const { return orient * glm::vec3(0, 0, -1); }
    glm::vec3 CameraRig3D::right() const { return orient * glm::vec3(1, 0, 0);  }
    glm::vec3 CameraRig3D::up() const { return orient * glm::vec3(0, 1, 0); }

    glm::mat4 CameraRig3D::view() const {
        glm::mat4 R = glm::toMat4(glm::conjugate(orient));
        glm::mat4 T = glm::translate(glm::mat4(1.0f), -glm::vec3(pos));
        return R * T;
    }

    glm::mat4 CameraRig3D::viewRotation() const {
        return glm::toMat4(glm::conjugate(orient));
    }

    glm::mat4 CameraRig3D::VPcr(float aspect, float nearKm) const {
        return proj(aspect, nearKm) * viewRotation();
    }

    glm::mat4 CameraRig3D::proj(float aspect, float nearKm) const {
        const float farKm = 1.0e12f;
        return glm::perspective(glm::radians(fovDeg), std::max(0.001f, aspect), nearKm, farKm);
    }

    void CameraRig3D::setDragFocusDepth(float depthKm) { dragDepthKm_ = depthKm; }
    void CameraRig3D::setDragFocusDepth(std::optional<float> depthKm) { dragDepthKm_ = depthKm; }

    static inline float smoothTo(float cur, float target, float hl, float dt) {
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

        const bool zHeld = kb.isDown(KeyCode::Z);

        // Update yaw/pitch quaternion from angles
        auto clampPitch = [](float p) { return std::clamp(p, -89.9f, 89.9f); };
        pitchDeg = clampPitch(pitchDeg);
        orient = glm::angleAxis(glm::radians(yawDeg), glm::vec3(0, 1, 0))
            * glm::angleAxis(glm::radians(pitchDeg), glm::vec3(1, 0, 0));

        // Mouse
        const bool lmb = mouse.isDown(Interstellar::Input::MouseButton::Left);
        const bool rmb = mouse.isDown(Interstellar::Input::MouseButton::Right);
        const auto  mpos = mouse.position();
        float mx = mpos.x, my = mpos.y;

        // Mouse wheel: dolly or FOV zoom (if 'Z' held).
        // Suppressed while RMB is held so that RMB drag never changes the camera position.
        const float wheel = mouse.wheel().y;
        if (wheel != 0.0f && !rmb) {
            if (zHeld) {
                targetFovDeg = std::clamp(targetFovDeg - wheel * 2.0f, params_.minFov, params_.maxFov);
            }
            else {
                // Dolly: toward focus if present, else along forward
                float step = wheel * params_.dollyKmPerNotch;
                if (focusWorld_) {
                    glm::dvec3 to   = *focusWorld_ - pos;
                    double     dist = glm::length(to);
                    glm::dvec3 dir  = (dist > 1e-6) ? (to / dist) : glm::dvec3(forward());
                    float scaled = step * (1.0f + 0.25f * std::sqrt(
                        std::max(float(dist), 0.0f) / std::max(1.0f, params_.dollyKmPerNotch)));
                    if (scaled > 0.0f) scaled = std::min(scaled, std::max(0.0f, float(dist) * 0.98f));
                    targetPos += dir * double(scaled);
                }
                else {
                    targetPos += glm::dvec3(forward()) * double(step);
                }
            }
        }

        // LMB drag: in-place look (same as RMB — the only way to visually move stars at
        // astronomical scale; translational pan is imperceptible at these distances)
        if (lmb && !drag_->lmbPrev) {
            drag_->lastX = mx; drag_->lastY = my;
        }
        if (lmb) {
            float dx = mx - drag_->lastX;
            float dy = my - drag_->lastY;
            drag_->lastX = mx; drag_->lastY = my;
            yawDeg   += dx * params_.yawSensDegPerPx;
            pitchDeg  = clampPitch(pitchDeg + dy * params_.pitchSensDegPerPx);
            orient = glm::angleAxis(glm::radians(yawDeg),   glm::vec3(0, 1, 0))
                   * glm::angleAxis(glm::radians(pitchDeg), glm::vec3(1, 0, 0));
        }
        drag_->lmbPrev = lmb;

        // RMB drag: pure view rotation (yaw/pitch only). targetPos is never touched here,
        // so the observer's position in the universe stays fixed while looking around.
        if (rmb && !drag_->rmbPrev) {
            drag_->rLastX = mx;
            drag_->rLastY = my;
        }
        if (rmb) {
            float dx = mx - drag_->rLastX;
            float dy = my - drag_->rLastY;
            drag_->rLastX = mx;
            drag_->rLastY = my;
            yawDeg   += dx * params_.yawSensDegPerPx;
            pitchDeg  = clampPitch(pitchDeg + dy * params_.pitchSensDegPerPx);
            orient = glm::angleAxis(glm::radians(yawDeg),   glm::vec3(0, 1, 0))
                   * glm::angleAxis(glm::radians(pitchDeg), glm::vec3(1, 0, 0));
        }
        drag_->rmbPrev = rmb;

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
            targetPos += glm::dvec3(glm::normalize(move)) * double(speed * float(dt));
        }
    }

    void CameraRig3D::update(double dt) {
        double k = std::pow(0.5, dt / std::max(1e-6, double(params_.posHL)));
        pos = glm::mix(targetPos, pos, k);
        fovDeg = smoothTo(fovDeg, targetFovDeg, params_.fovHL, float(dt));
    }

} // namespace Interstellar::Engine::Cameras

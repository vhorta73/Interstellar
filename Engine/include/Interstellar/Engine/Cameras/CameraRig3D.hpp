#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <optional>
#include <memory>

namespace Interstellar {
    namespace Input {
        class IKeyboard;
        class IMouse;
    }
}

namespace Interstellar::Engine::Cameras {

    struct CameraRigParams {
        float minFov = 25.0f;
        float maxFov = 90.0f;
        float posHL = 0.12f;
        float fovHL = 0.18f;
        float panBase = 0.7f;                // km/sec for WASD
        float dollyKmPerNotch = 1.0e8f;      // km / wheel notch (scaled if focus set)
        float yawSensDegPerPx = 0.12f;       // Space+LMB look
        float pitchSensDegPerPx = 0.12f;     // Space+LMB look
        float dragMult = 1.0f;               // 2D pan feel (LMB without Space)
    };

    class CameraRig3D {
    public:
        explicit CameraRig3D(const CameraRigParams& p = {}) : params_(p) {}
        ~CameraRig3D();

        // Pose
        glm::vec3 pos{ 0.0f };
        float     yawDeg = 0.0f;
        float     pitchDeg = 0.0f;
        float     fovDeg = 60.0f;
        glm::quat orient{ 1,0,0,0 }; // derived from yaw/pitch (zero roll)

        // Smoothed targets
        glm::vec3 targetPos = pos;
        float     targetFovDeg = fovDeg;

        // Input & smoothing
        void handleInput(const Interstellar::Input::IKeyboard& kb,
            Interstellar::Input::IMouse& mouse,
            int viewportW, int viewportH,
            double dt);
        void update(double dt);

        // Matrices & helpers
        glm::mat4 view() const;
        glm::mat4 proj(float aspect) const;
        glm::mat4 VP(float aspect) const { return proj(aspect) * view(); }
        glm::vec3 eye() const { return pos; }

        glm::vec3 forward() const;
        glm::vec3 right()   const;
        glm::vec3 up()      const;

        const CameraRigParams& params() const { return params_; }
        CameraRigParams& params() { return params_; }

        // Drag scaling depth (km) for 2D pan
        void setDragFocusDepth(float depthKm);
        void setDragFocusDepth(std::optional<float> depthKm);
        void clearDragFocusDepth() { dragDepthKm_.reset(); }
        std::optional<float> dragFocusDepth() const { return dragDepthKm_; }

        // Optional focus point for wheel
        void setFocusWorld(const glm::vec3& p) { focusWorld_ = p; }
        void clearFocusWorld() { focusWorld_.reset(); }
        std::optional<glm::vec3> focusWorld() const { return focusWorld_; }

    private:
        CameraRigParams params_;

        struct DragImpl;
        struct DragDeleter { void operator()(DragImpl*) noexcept; };
        std::unique_ptr<DragImpl, DragDeleter> drag_;

        int  lastViewportW_ = 0;
        int  lastViewportH_ = 0;

        bool rotateActive_ = false; // true only while Space+LMB held

        std::optional<float>     dragDepthKm_;
        std::optional<glm::vec3> focusWorld_;
    };

} // namespace Interstellar::Engine::Cameras

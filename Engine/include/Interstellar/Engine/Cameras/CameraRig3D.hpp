#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cstdint>
#include <Interstellar/Input/Input.hpp>

namespace Interstellar::Engine {

    struct CameraRigParams {
        // limits
        float minZ = 5.0f;
        float maxZ = 2.0e6f;
        float minFov = 25.0f;
        float maxFov = 90.0f;
        // smoothing (half-lives in seconds)
        float posHL = 0.12f;
        float zHL = 0.18f;
        float fovHL = 0.18f;
        // speeds
        float panBase = 0.7f;  // world units / sec (XY)
        float dollySpeed = 0.15f; // wheel speed for Z/fov control
    };

    class CameraRig3D {
    public:
        explicit CameraRig3D(const CameraRigParams& p = {}) : params_(p) {}

        // State (current)
        glm::vec2 center{ 0.0f, 0.0f }; // world X,Y
        float     z = 2000.0f;   // camera distance along +Z (looking towards -Z)
        float     fovDeg = 60.0f;

        // Targets (smoothed towards)
        glm::vec2 targetCenter = center;
        float     targetZ = z;
        float     targetFovDeg = fovDeg;

        // Input handling & smoothing
        void handleInput(const Interstellar::Input::IKeyboard& kb,
            const Interstellar::Input::IMouse& mouse,
            int viewportW, int viewportH,
            double dt);
        void update(double dt);

        // Matrices & camera vectors
        glm::mat4 view() const;
        glm::mat4 proj(float aspect) const;
        glm::mat4 VP(float aspect) const { return proj(aspect) * view(); }
        glm::vec3 eye() const { return { center.x, center.y, z }; }

        // Access params
        const CameraRigParams& params() const { return params_; }
        CameraRigParams& params() { return params_; }

    private:
        CameraRigParams params_;
    };

} // namespace Interstellar::Engine

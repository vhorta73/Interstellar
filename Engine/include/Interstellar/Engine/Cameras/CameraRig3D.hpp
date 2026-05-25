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

    /**
     * @ingroup Engine
     * @brief Tunable parameters for CameraRig3D motion and sensitivity.
     * @since 1.0
     */
    struct CameraRigParams {
        float minFov = 25.0f;               ///< Minimum field of view in degrees.
        float maxFov = 90.0f;               ///< Maximum field of view in degrees.
        float posHL  = 0.12f;               ///< Position smoothing half-life (seconds).
        float fovHL  = 0.18f;               ///< FOV smoothing half-life (seconds).
        float panBase = 0.7f;               ///< WASD translation speed (km/s).
        float dollyKmPerNotch = 1.0e8f;     ///< Wheel dolly distance per notch (km); scales with focus distance.
        float yawSensDegPerPx = 0.12f;      ///< Look sensitivity: yaw degrees per pixel (Space+LMB drag).
        float pitchSensDegPerPx = 0.12f;    ///< Look sensitivity: pitch degrees per pixel (Space+LMB drag).
        float dragMult = 1.0f;              ///< 2-D pan feel multiplier (LMB drag without Space).
    };

    /**
     * @ingroup Engine
     * @brief First-person 3-D camera rig with exponential smoothing, WASD movement,
     *        mouse-look (Space+LMB), 2-D pan (LMB), and focus-aware wheel dolly.
     *
     * Pose is split into *target* values (instantly updated by input) and *current*
     * values (smoothed toward targets each @ref update call).  Callers must call
     * @ref handleInput then @ref update every frame in that order.
     *
     * @since 1.0
     */
    class CameraRig3D {
    public:
        /**
         * @ingroup Engine
         * @brief Construct a rig with optional parameter override.
         * @param p [in] CameraRigParams - Initial tuning parameters.
         * @throws None
         * @complexity O(1)
         * @thread_safety Not thread-safe.
         * @since 1.0
         */
        explicit CameraRig3D(const CameraRigParams& p = {}) : params_(p) {}

        /**
         * @ingroup Engine
         * @brief Destructor.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        ~CameraRig3D();

        // ---- Pose (current smoothed values) ----
        glm::dvec3 pos{ 0.0 };          ///< Smoothed world position (km), double precision for sub-metre accuracy at stellar scale.
        float      yawDeg   = 0.0f;     ///< Yaw angle in degrees (applied before pitch).
        float      pitchDeg = 0.0f;     ///< Pitch angle in degrees, clamped to ±89.9°.
        float      fovDeg   = 60.0f;    ///< Smoothed vertical field of view in degrees.
        glm::quat  orient{ 1,0,0,0 };   ///< Orientation quaternion derived from yaw/pitch (zero roll).

        // ---- Smoothing targets (set by handleInput) ----
        glm::dvec3 targetPos    = pos;       ///< Position target; smoothed toward by @ref update.
        float      targetFovDeg = fovDeg;    ///< FOV target; smoothed toward by @ref update.

        /**
         * @ingroup Engine
         * @brief Read keyboard and mouse state, update orientation and motion targets.
         * @details Controls: WASD+Q/E translate; LMB drag and RMB drag both look (yaw/pitch);
         *          scroll wheel dollies (toward @ref focusWorld if set) — suppressed while RMB
         *          is held so the observer position never changes during a RMB look-drag;
         *          Z+wheel adjusts FOV.  Smoothing is deferred to @ref update.
         * @param kb        [in] IKeyboard - Keyboard state for the current frame.
         * @param mouse     [in] IMouse    - Mouse state for the current frame.
         * @param viewportW [in] int       - Viewport width in pixels.
         * @param viewportH [in] int       - Viewport height in pixels.
         * @param dt        [in] double    - Delta time in seconds since last frame.
         * @throws None
         * @complexity O(1)
         * @thread_safety Not thread-safe; call from the simulation thread only.
         * @reentrancy No
         * @since 1.0
         */
        void handleInput(const Interstellar::Input::IKeyboard& kb,
            Interstellar::Input::IMouse& mouse,
            int viewportW, int viewportH,
            double dt);

        /**
         * @ingroup Engine
         * @brief Advance smoothing: interpolate @ref pos toward @ref targetPos and
         *        @ref fovDeg toward @ref targetFovDeg using exponential half-life decay.
         * @param dt [in] double - Delta time in seconds since last frame.
         * @throws None
         * @complexity O(1)
         * @thread_safety Not thread-safe.
         * @since 1.0
         */
        void update(double dt);

        /**
         * @ingroup Engine
         * @brief Compute the view matrix from the current smoothed pose.
         * @return glm::mat4 - View (world-to-camera) matrix.
         * @throws None
         * @complexity O(1)
         * @thread_safety Yes (read-only after update).
         * @reentrancy Yes
         * @since 1.0
         */
        glm::mat4 view() const;

        /**
         * @ingroup Engine
         * @brief Compute the perspective projection matrix.
         * @param aspect  [in] float - Viewport aspect ratio (width / height).
         * @param nearKm  [in] float - Near-plane distance in km (default 1 km). Scale down to
         *                             sub-kilometre values when close to a surface to avoid
         *                             depth fighting; @ref UniverseScene sets this dynamically.
         * @return glm::mat4 - Projection matrix with far=1e12 km.
         * @throws None
         * @complexity O(1)
         * @thread_safety Yes (read-only).
         * @reentrancy Yes
         * @since 1.0
         */
        glm::mat4 proj(float aspect, float nearKm = 1.0f) const;

        /**
         * @ingroup Engine
         * @brief Compute the combined view-projection matrix (world-space geometry).
         * @param aspect [in] float - Viewport aspect ratio (width / height).
         * @return glm::mat4 - Combined VP matrix (proj * view).
         * @throws None
         * @complexity O(1)
         * @thread_safety Yes (read-only).
         * @reentrancy Yes
         * @since 1.0
         */
        glm::mat4 VP(float aspect) const { return proj(aspect) * view(); }

        /**
         * @ingroup Engine
         * @brief Rotation-only view matrix (no translation).
         * @details Use with camera-relative geometry: subtract @ref eyeD from every world
         *          position before uploading to the GPU, then transform with this matrix.
         *          Eliminates floating-point cancellation that occurs when the camera is far
         *          from the world origin.
         * @return glm::mat4 - Rotation-only view matrix (camera at origin).
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        glm::mat4 viewRotation() const;

        /**
         * @ingroup Engine
         * @brief View-projection matrix for camera-relative geometry.
         * @details Combines @ref proj with @ref viewRotation.  Geometry must be expressed
         *          relative to the camera (i.e. @c worldPos - @ref eyeD cast to float).
         * @param aspect [in] float - Viewport aspect ratio (width / height).
         * @param nearKm [in] float - Near-plane distance in km (default 1 km).
         * @return glm::mat4 - VP matrix for camera-relative geometry.
         * @throws None
         * @complexity O(1)
         * @thread_safety Yes (read-only).
         * @since 1.0
         */
        glm::mat4 VPcr(float aspect, float nearKm = 1.0f) const;

        /**
         * @ingroup Engine
         * @brief Get the camera eye position in world space (float, for coarse uses).
         * @return glm::vec3 - Current smoothed position cast to float (km).
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        glm::vec3 eye() const { return glm::vec3(pos); }

        /**
         * @ingroup Engine
         * @brief Get the camera eye position in world space, double precision.
         * @details Use this for any calculation that must remain accurate at sub-kilometre
         *          scale (e.g. star distance queries, near-plane computation, HUD readout).
         * @return glm::dvec3 - Current smoothed position (km).
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        glm::dvec3 eyeD() const { return pos; }

        /**
         * @ingroup Engine
         * @brief Get the camera's forward unit vector in world space.
         * @return glm::vec3 - Normalised forward direction.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        glm::vec3 forward() const;

        /**
         * @ingroup Engine
         * @brief Get the camera's right unit vector in world space.
         * @return glm::vec3 - Normalised right direction.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        glm::vec3 right()   const;

        /**
         * @ingroup Engine
         * @brief Get the camera's up unit vector in world space.
         * @return glm::vec3 - Normalised up direction.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        glm::vec3 up()      const;

        /**
         * @ingroup Engine
         * @brief Read-only access to motion/sensitivity parameters.
         * @return const CameraRigParams& - Current parameters.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        const CameraRigParams& params() const { return params_; }

        /**
         * @ingroup Engine
         * @brief Mutable access to motion/sensitivity parameters.
         * @return CameraRigParams& - Reference to current parameters.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        CameraRigParams& params() { return params_; }

        /**
         * @ingroup Engine
         * @brief Set the depth (km) used to scale 2-D pan so dragged objects appear screen-anchored.
         * @param depthKm [in] float - Distance to the drag plane in km.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        void setDragFocusDepth(float depthKm);

        /**
         * @ingroup Engine
         * @brief Set or clear the drag focus depth via an optional.
         * @param depthKm [in] std::optional<float> - Depth in km, or std::nullopt to clear.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        void setDragFocusDepth(std::optional<float> depthKm);

        /**
         * @ingroup Engine
         * @brief Clear the drag focus depth, reverting to the default pan scale.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        void clearDragFocusDepth() { dragDepthKm_.reset(); }

        /**
         * @ingroup Engine
         * @brief Query the current drag focus depth.
         * @return std::optional<float> - Depth in km, or std::nullopt if unset.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        std::optional<float> dragFocusDepth() const { return dragDepthKm_; }

        /**
         * @ingroup Engine
         * @brief Set a world-space focus point used by the wheel dolly to approach a specific target.
         * @param p [in] glm::dvec3 - Target world position in km.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        void setFocusWorld(const glm::dvec3& p) { focusWorld_ = p; }

        /**
         * @ingroup Engine
         * @brief Clear the world-space focus point; wheel dollies along the forward vector instead.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        void clearFocusWorld() { focusWorld_.reset(); }

        /**
         * @ingroup Engine
         * @brief Query the current world-space focus point.
         * @return std::optional<glm::dvec3> - Focus position in km, or std::nullopt if unset.
         * @throws None
         * @complexity O(1)
         * @since 1.0
         */
        std::optional<glm::dvec3> focusWorld() const { return focusWorld_; }

    private:
        CameraRigParams params_;

        struct DragImpl;
        struct DragDeleter { void operator()(DragImpl*) noexcept; };
        std::unique_ptr<DragImpl, DragDeleter> drag_;

        int  lastViewportW_ = 0;
        int  lastViewportH_ = 0;

        bool rotateActive_ = false;

        std::optional<float>      dragDepthKm_;
        std::optional<glm::dvec3> focusWorld_;
    };

} // namespace Interstellar::Engine::Cameras

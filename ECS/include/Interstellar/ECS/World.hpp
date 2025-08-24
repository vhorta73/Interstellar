#pragma once
#include <vector>
#include <cstdint>

namespace Interstellar::ECS {

    /**
     * @file
     * @brief Minimal ECS slice: contiguous Transform/Velocity arrays with index-based entity IDs.
     * @ingroup ECS
     * @since 1.0
     *
     * Design notes:
     * - Entity is an index into parallel arrays (no indirection).
     * - Creation appends; there is no deletion or compaction.
     * - simulate(dt) performs simple Euler integration: p += v * dt.
     *
     * Thread-safety:
     * - Not thread-safe. Callers must provide external synchronization.
     */

     /**
      * @brief Stable entity identifier.
      * @details In this implementation, Entity is the index of a row across component arrays.
      * @ingroup ECS
      */
    using Entity = std::uint32_t;

    /**
     * @brief Position component in 2D.
     * @ingroup ECS
     */
    struct Transform {
        float x, y;  ///< Position in world units.
    };

    /**
     * @brief Velocity component in 2D.
     * @ingroup ECS
     */
    struct Velocity {
        float vx, vy;  ///< Velocity in world units per second.
    };

    /**
     * @brief Simple world holding Transform and Velocity components.
     * @ingroup ECS
     *
     * Storage model:
     * - Two parallel vectors: transforms[i] pairs with velocities[i].
     * - Entity values are indices into these vectors.
     * - No remove; IDs are never recycled.
     */
    class World {
    public:
        /**
         * @brief Create a new entity with Transform and Velocity.
         * @param x  Initial position X (world units).
         * @param y  Initial position Y (world units).
         * @param vx Initial velocity X (units per second).
         * @param vy Initial velocity Y (units per second).
         * @return Entity ID assigned to the new row (index into component arrays).
         *
         * @complexity Amortized O(1) appends; may reallocate.
         * @throws std::bad_alloc on vector growth.
         * @post getTransforms().size() has increased by 1 and the returned ID
         *       is less than the new size.
         */
        Entity create(float x, float y, float vx, float vy) {
            Entity id = static_cast<Entity>(transforms.size());
            transforms.push_back({ x,y });
            velocities.push_back({ vx,vy });
            return id;
        }

        /**
         * @brief Advance simulation by dt seconds with Euler integration.
         * @param dt Delta time in seconds.
         *
         * @details For each i: x += vx * dt; y += vy * dt.
         * @pre dt can be positive, zero, or negative; caller controls stability.
         * @complexity O(N) where N == entity count.
         * @throws None.
         * @warning No collision or bounds handling; purely kinematic.
         */
        void simulate(double dt) {
            for (std::size_t i = 0; i < transforms.size(); ++i) {
                transforms[i].x += velocities[i].vx * static_cast<float>(dt);
                transforms[i].y += velocities[i].vy * static_cast<float>(dt);
            }
        }

        /**
         * @brief Read-only view of all Transform components.
         * @return Const reference to the internal Transform array.
         *
         * @complexity O(1).
         * @throws None.
         * @warning Reference is invalidated if the world mutates in ways that
         *          reallocate the underlying storage (e.g., create may reallocate).
         */
        const std::vector<Transform>& getTransforms() const { return transforms; }

    private:
        // Parallel arrays; indices form the entity ID.
        std::vector<Transform> transforms;
        std::vector<Velocity> velocities;
    };

} // namespace Interstellar::ECS

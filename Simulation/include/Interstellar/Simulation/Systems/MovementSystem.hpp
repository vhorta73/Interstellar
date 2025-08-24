#pragma once
#include "Interstellar/ECS/World.hpp"

namespace Interstellar::Simulation {

    /**
     * @file
     * @brief Thin simulation system wrappers over ECS.
     * @ingroup Simulation
     * @since 1.0
     */

     /**
      * @brief Integrate world positions by velocity for dt seconds.
      * @ingroup Simulation
      *
      * @param world ECS world to advance. Assumes each entity has matching Transform and Velocity rows.
      * @param dt    Time step in seconds (positive, zero, or negative). Caller controls stability.
      *
      * @details
      * Delegates to Interstellar::ECS::World::simulate(dt). Uses simple Euler integration:
      * x += vx * dt; y += vy * dt for each entity.
      *
      * @throws None
      * @complexity O(N) where N is the number of entities.
      * @thread_safety Not thread-safe; synchronize externally if used from multiple threads.
      * @see Interstellar::ECS::World::simulate
      * @since 1.0
      */
    inline void MovementSystem(Interstellar::ECS::World& world, double dt) {
        world.simulate(dt);
    }

} // namespace Interstellar::Simulation

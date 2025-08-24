#pragma once
#include <functional>
#include <chrono>

namespace Interstellar::Engine {

    struct EngineConfig {
        double simHz = 60.0;   // fixed simulation tick rate
    };

    class Engine {
    public:
        explicit Engine(const EngineConfig& cfg = {}) : config(cfg) {}

        // run until shouldClose() returns true
        template<class SimFn, class RenderFn, class ShouldCloseFn>
        void run(SimFn&& sim, RenderFn&& render, ShouldCloseFn&& shouldClose) {
            using clock = std::chrono::high_resolution_clock;
            using dur = std::chrono::duration<double>;

            const double dt = 1.0 / config.simHz;
            double accumulator = 0.0;
            auto last = clock::now();

            while (!shouldClose()) {
                auto now = clock::now();
                double frameTime = std::chrono::duration_cast<dur>(now - last).count();
                last = now;
                accumulator += frameTime;

                // fixed-step simulation
                while (accumulator >= dt) {
                    sim(dt);
                    accumulator -= dt;
                }

                // render (can use partial accumulator if you like)
                render(frameTime);
            }
        }

    private:
        EngineConfig config;
    };

} // namespace Interstellar::Engine

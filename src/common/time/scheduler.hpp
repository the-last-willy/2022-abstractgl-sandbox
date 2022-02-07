#pragma once

#include "clock.hpp"

#include <cmath>
#include <functional>
#include <thread>

struct Scheduler {
    std::function<bool()> is_running = [](){ return true; };

    std::function<void()> on_init = [](){};

    float time_per_render = 1.f / 60.f;
    float time_since_render = 0.f;
    std::function<void()> on_render = [](){};

    float time_per_update = 1.f / 60.f;
    float time_since_update = 0.f;
    std::function<void()> on_update = [](){};
};

inline
void run(Scheduler& s) {
    auto c = Clock();
    s.on_init();
    while(s.is_running()) {
        {
            auto time_elapsed = c.restart().count();
            s.time_since_render += time_elapsed;
            s.time_since_update += time_elapsed;
        }
        if(s.time_since_update >= s.time_per_update) {
            s.time_since_update -= s.time_per_update;
            s.on_update();
        }
        if(s.time_since_render >= s.time_per_render) {
            // Skip accumulated renders.
            s.time_since_render = std::fmod(s.time_since_render, s.time_per_render);
            s.on_render();
        }
        std::this_thread::sleep_for(std::chrono::seconds(0));
    }
}

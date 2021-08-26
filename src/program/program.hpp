#pragma once

#include "clock.hpp"
#include "window.hpp"

struct Program {
    Window window = {};

    // Seconds.
    std::chrono::duration<float> time_per_update
    = std::chrono::duration<float>(1.f / 60.f);

    virtual void init() = 0;

    virtual void update(float dt) = 0;

    virtual void render() = 0;
};

void run(Program& program) {
    program.init();

    auto clock = Clock();

    auto time_since_update = std::chrono::duration<float>(0.f);
    while(!glfwWindowShouldClose(program.window.window)) {
        time_since_update += clock.restart();
        if(time_since_update >= program.time_per_update) {
            time_since_update -= program.time_per_update;
            program.update(1.f / 60.f);
        }

        program.render();

        glfwSwapBuffers(program.window.window);
        glfwPollEvents();
    }
}

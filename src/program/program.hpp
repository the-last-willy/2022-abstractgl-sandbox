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

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(program.window.window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        program.render();

        // ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        glfwGetFramebufferSize(program.window.window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(program.window.window);
    }
}

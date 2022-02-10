#include "common/dependency/glfw_opengl.hpp"
#include "common/dependency/imgui_glfw_opengl.hpp"
#include "common/dependency/imgui_glfw_opengl.hpp"
#include "common/time/scheduler.hpp"
#include "hello_triangle.hpp"

#include <gsl/gsl>
#include <cstdlib>
#include <iostream>

static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::ignore = scancode;
    std::ignore = mods;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void throwing_main() {
    glfwSetErrorCallback(glfw_error_callback);
    
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        std::exit(-1);
    }
    auto terminate_glfw_at_the_end = gsl::finally([]() {
        glfwTerminate();
    });
    
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL", NULL, NULL);
    if(window == NULL) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        std::exit(-1);
    }
    auto destroy_glfw_window_at_the_end = gsl::finally([&]() {
        glfwDestroyWindow(window);
    });

    glfwSetKeyCallback(window, glfw_key_callback);
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK) {
        std::cerr << "Failed to initalize GLEW." << std::endl;
        std::exit(-1);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    auto clear_imgui_at_the_end = gsl::finally([]() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    });

    { // Running application.
        // Must be done as soon as possible but should be in program instead of here.
        { // Debug configuration.
            glDebugMessageCallback(&opengl_debug_message_callback, NULL);
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        }

        auto app = HelloTriangle();

        auto scheduler = Scheduler();
        scheduler.is_running = [&]() {
            return not glfwWindowShouldClose(window);
        };
        scheduler.on_init = [&]() {
            init(app);
        };
        scheduler.time_per_render = 1.f / 60.f;
        scheduler.on_render = [&]() {
            

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::ShowDemoWindow();
            
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            glClearColor(
                clear_color.x * clear_color.w,
                clear_color.y * clear_color.w,
                clear_color.z * clear_color.w,
                clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            render(app);

            glfwSwapBuffers(window);
        };
        scheduler.time_per_update = 1.f / 60.f;
        scheduler.on_update = [&]() {
            glfwPollEvents();

            update(app);
        };
        run(scheduler);
    }
}

int main() {
    try {
        throwing_main();
        return 0;
    } catch(const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
        return -1;
    } catch(...) {
        std::cerr << "Unhandled exception." << std::endl;
        return -1;
    }
}

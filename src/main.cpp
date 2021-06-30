#include "matrix/all.hpp"
#include "shader/all.hpp"
#include "vector/all.hpp"
#include "file.hpp"
#include "pi.hpp"
#include "root.hpp"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include "linmath.h"
 
#include <stdlib.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
 
using namespace tlw;

struct Vertex {
    float x, y;
    float r, g, b;
};

const Vertex vertices[6] =
{
    { -1.f, -1.f, 1.f, 0.f, 0.f },
    { -1.f,  1.f, 0.f, 1.f, 0.f },
    {  1.f, -1.f, 0.f, 0.f, 1.f },
    { -1.f,  1.f, 1.f, 1.f, 0.f },
    {  1.f, -1.f, 1.f, 0.f, 1.f },
    {  1.f,  1.f, 0.f, 1.f, 1.f }
};

const std::string vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";
 
static void error_callback(int error, const char* description)
{
    (void) error;
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void) scancode;
    (void) mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

const std::string fragment_shader_filepath = root + "src/shader.glsl";

int throwing_main(void) {
    const std::string fragment_shader_text = file(fragment_shader_filepath);

    GLFWwindow* window;
    GLuint vertex_buffer, program;
    GLint mvp_location, vpos_location, vcol_location;
 
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
 
    window = glfwCreateWindow(1280, 720, "First stream yay !", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
 
    glfwSetKeyCallback(window, key_callback);
 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize OpenGL context");
        return -1;
    }
  
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    auto vertex_shader = Shader(GL_VERTEX_SHADER);
    {
        source(vertex_shader, vertex_shader_text);
        auto warnings = compile(vertex_shader);
        std::cout << warnings << std::endl;
    }
 
    auto fragment_shader = Shader(GL_FRAGMENT_SHADER);
    {
        source(fragment_shader, fragment_shader_text);
        auto warnings = compile(fragment_shader);
        std::cout << warnings << std::endl;
    }
    
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
 
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
 
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));
 
    float angle_y = 0.f;
    auto camera_position = tlw::vec3(0.f, 1.5f, -5.f);

    while (!glfwWindowShouldClose(window)) {
        if(glfwGetKey(window, GLFW_KEY_A)) {
            angle_y += 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_D)) {
            angle_y -= 0.02f;
        }
        if(glfwGetKey(window, GLFW_KEY_S)) {
            camera_position = camera_position
            + xyz(rotation_y(angle_y)
            * tlw::vec4(0.f, 0.f, -0.2f, 1.f));
        }
        if(glfwGetKey(window, GLFW_KEY_R)) {
            auto src = file(fragment_shader_filepath);
            source(fragment_shader, src);
            auto warnings = compile(fragment_shader);
            std::cout << warnings << std::endl;
            glLinkProgram(program);
        }
        if(glfwGetKey(window, GLFW_KEY_W)) {
            camera_position = camera_position
            + xyz(rotation_y(angle_y)
            * tlw::vec4(0.f, 0.f, 0.2f, 1.f));
        }

        float ratio;
        int width, height;
        mat4x4 m, p, mvp;
 
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
 
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);
 
        auto camera =  rotation_y(angle_y) * translation(camera_position);

        glProgramUniformMatrix4fv(
            program,
            glGetUniformLocation(program, "camera"),
            1,
            false,
            camera.elements.data());

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(Vertex));
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

int main() {
    try {
        return throwing_main();
    } catch(const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Unhandled exception." << std::endl;
    }
    return -1;
}

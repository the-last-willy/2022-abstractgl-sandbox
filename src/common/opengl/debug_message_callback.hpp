#pragma once

#include <stdexcept>
#include <iostream>

void opengl_debug_message_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    std::ignore = source;
    std::ignore = type;
    std::ignore = id;
    std::ignore = length;
    std::ignore = userParam;

    std::cout << "OpenGL: " << message << std::endl;
    if(severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        throw std::runtime_error("OpenGL error.");
    }
}

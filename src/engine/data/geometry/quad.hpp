#pragma once

#include "dependencies.hpp"

inline
auto make_quad_geometry() {
    auto geometry = Geometry();
    geometry.primitive_count = agl::Count<GLsizei>(6);
    { // Indices.
        auto data = std::array{
            0u, 1u, 2u,
            1u, 3u, 2u};
        auto b = agl::buffer();
        agl::storage(b, std::span<GLuint>(data), 0);
        geometry.indexes = std::move(b);
    }
    { // 'normal3'
        auto data = std::array{
            agl::vec3(0, 1, 0), agl::vec3(0, 1, 0),
            agl::vec3(0, 1, 0), agl::vec3(0, 1, 0),};
        auto b = agl::buffer();  
        agl::storage(b, std::span(data), 0);
        geometry.attributes["normal3"] = {
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    { // 'position3'
        auto data = std::array{
            agl::vec3(-1, 0, -1), agl::vec3(-1, 0, 1),
            agl::vec3( 1, 0, -1), agl::vec3( 1, 0, 1),};
        auto b = agl::buffer();  
        agl::storage(b, std::span(data), 0);
        geometry.attributes["position3"] = {
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    { // 'texcoords2'
        auto data = std::array{
            agl::vec2(0, 0), agl::vec2(0, 1),
            agl::vec2(1, 0), agl::vec2(1, 1),};
        auto b = agl::buffer();  
        agl::storage(b, std::span(data), 0);
        geometry.attributes["texcoords2"] = {
            .buffer = std::move(b),

            .size = agl::Size<GLint>(2),
            .stride = agl::Stride<GLsizei>(2 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    return geometry;
}

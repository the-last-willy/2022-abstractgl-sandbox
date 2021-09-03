#pragma once

#include "dependencies.hpp"

inline
auto make_cylinder_geometry() {
    auto geometry = Geometry();
    constexpr auto resolution = 10;
    geometry.primitive_count = agl::Count<GLsizei>(3 * 2 * resolution);
    { // Indices.
        auto data = std::array<GLuint, 3 * 2 * resolution>{};
        for(int i = 0; i < resolution; ++i) {
            // Lower triangle.
            data[2 * 3 * i + 0 + 0] = i;
            data[2 * 3 * i + 0 + 1] = (i + 1) % resolution;
            data[2 * 3 * i + 0 + 2] = i + resolution;
            // Upper triangle.
            data[2 * 3 * i + 3 + 0] = (i + 1) % resolution + resolution;
            data[2 * 3 * i + 3 + 1] = i + resolution;
            data[2 * 3 * i + 3 + 2] = (i + 1) % resolution;
        }
        auto b = agl::buffer();
        agl::storage(b, std::span(data));
        geometry.indexes = std::move(b);
    }
    { // Normals.
        auto data = std::array<agl::Vec3, 2 * resolution>{};
        for(int i = 0; i < resolution; ++i) {
            auto c = std::cos(agl::tau * i / resolution);
            auto s = std::sin(agl::tau * i / resolution);
            data[i             ] = agl::vec3(c, 0.f, s);
            data[i + resolution] = agl::vec3(c, 0.f, s);
        }
        auto b = agl::buffer();
        agl::storage(b, std::span(data));
        geometry.attributes["normal3"] = AttributeBuffer{
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    { // Positions.
        auto data = std::array<agl::Vec3, 2 * resolution>{};
        for(int i = 0; i < resolution; ++i) {
            auto c = std::cos(agl::tau * i / resolution);
            auto s = std::sin(agl::tau * i / resolution);
            data[i             ] = agl::vec3(c, -1.f, s);
            data[i + resolution] = agl::vec3(c,  1.f, s);
        }
        auto b = agl::buffer();
        agl::storage(b, std::span(data));
        geometry.attributes["position3"] = AttributeBuffer{
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    return geometry;
}

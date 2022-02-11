#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"

#include <agl/constant/all.hpp>

#include <cmath>
#include <vector>

namespace gizmo {

struct Solid_UV_Sphere {
    gl::BufferObj elements;
    gl::BufferObj normals_positions;

    GLenum mode = GL_TRIANGLES;
    GLint first = 0;
    GLenum type = GL_UNSIGNED_INT;
    GLsizei count;

    Solid_UV_Sphere(int n0, int n1) {
        using agl::constant::pi;
        using agl::constant::tau;

        count = 3 * 2 * n0 * n1;
        
        { // Elements.
            auto elems = std::vector<GLuint>();
            elems.reserve(std::size_t(count));
            for(int i0 = 0; i0 < n0; ++i0) {
                for(int j = 0; j < n1; ++j) {
                    auto i1 = (i0 + 1) % n0;
                    // Lower triangle.
                    elems.push_back(i0 * (n1 + 1) + (j + 0));
                    elems.push_back(i1 * (n1 + 1) + (j + 0));
                    elems.push_back(i0 * (n1 + 1) + (j + 1));
                    // Upper triangle.
                    elems.push_back(i0 * (n1 + 1) + (j + 1));
                    elems.push_back(i1 * (n1 + 1) + (j + 0));
                    elems.push_back(i1 * (n1 + 1) + (j + 1));
                }
            }
            gl::NamedBufferStorage(elements,
                elems,
                GL_NONE);
        }
        { // Normals, positions.
            auto vecs = std::vector<glm::vec3>();
            vecs.reserve(n0 * (n1 + 1));
            for(int i = 0; i < n0; ++i) {
                auto a = tau * float(i) / float(n0);
                for(int j = 0; j <= n1; ++j) {
                    auto b = pi * float(j) / float(n1);
                    vecs.emplace_back(
                        sin(b) * cos(a),
                        sin(b) * sin(a),
                        -cos(b));
                }
            }
            gl::NamedBufferStorage(normals_positions,
                vecs,
                GL_NONE);
        }
    }
};

}

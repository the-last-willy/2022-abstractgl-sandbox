#pragma once

#include "common/dependency/abstractgl_api_opengl.hpp"
#include "common/dependency/glm.hpp"

#include <agl/common/constant/all.hpp>

#include <cmath>
#include <vector>

struct Solid_UV_Sphere {
    gl::BufferObj elements;
    gl::BufferObj normals_positions;

    GLenum mode = GL_TRIANGLES;
    GLint first = 0;
    GLsizei count;

    Solid_UV_Sphere(int nx, int ny) {
        using agl::constant::pi;
        using agl::constant::tau;

        count = ;
        
        
        { // Elements.
            
        }
        { // Normals, positions.
            auto vectors = std::vector<glm::vec3>();
            vectors.reserve((nx + 1) * (ny + 1) + 2);
            // Lower cap.
            vectors.emplace_back(-1.f, 0.f, 0.f);
            // Middle vertices.
            for(int x = 0; x <= nx; ++x) {
                auto a = float(x + 1) / float(nx + 2) * pi;
                for(int y = 0; y <= ny; ++y) {
                    auto b = float(y) / float(ny + 1) * tau;
                    auto sa = std::sin(a);
                    vectors.emplace_back(
                        -std::cos(a),
                        sa * std::cos(b),
                        sa * std::sin(b));
                }
            }
            
            // Upper cap.
            vectors.emplace_back(+1.f, 0.f, 0.f);

            gl::NamedBufferStorage(normals_positions,
                vectors,
                GL_NONE);
        }
    }
};

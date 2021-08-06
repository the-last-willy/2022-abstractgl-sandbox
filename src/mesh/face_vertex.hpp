#pragma once

#include "vertex.hpp"

#include "engine/all.hpp"

#include <array>
#include <vector>

namespace mesh {

struct Triangle {
    std::array<unsigned, 3> vertices = {};

    constexpr
    auto operator[](std::size_t i) const {
        return vertices[i];
    }
};

struct FaceVertex {
    std::vector<Triangle> triangles = {};
    std::vector<Vertex> vertices = {};
};

inline
auto geometry(const FaceVertex& mesh) {
    auto g = Geometry();
    g.primitive_count = agl::Count<GLsizei>(GLsizei(3 * size(mesh.triangles)));
    { // Colors.
        auto b = agl::buffer();
        agl::storage(b, size(mesh.vertices) * sizeof(agl::Vec3), GL_MAP_WRITE_BIT);
        auto mapping = agl::map<agl::Vec3>(b, GL_WRITE_ONLY);
        auto idx = 0;
        for(auto& v : mesh.vertices) {
            mapping[idx++] = v.color;
        }
        agl::unmap(b);
        g.attributes["color_rgb"] = AttributeBuffer{
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(12),
            .type = GL_FLOAT,
        };
    }
    { // Indices.
        auto b = agl::buffer();
        agl::storage(b, size(mesh.triangles) * sizeof(mesh::Triangle), GL_MAP_WRITE_BIT);
        auto mapping = agl::map<GLuint>(b, GL_WRITE_ONLY);
        auto idx = 0;
        for(auto& t : mesh.triangles) {
            mapping[idx++] = t[0];
            mapping[idx++] = t[1];
            mapping[idx++] = t[2];
        }
        agl::unmap(b);
        g.indexes = std::move(b);
    }
    { // Normals.
        auto b = agl::buffer();
        agl::storage(b, size(mesh.vertices) * sizeof(agl::Vec3), GL_MAP_WRITE_BIT);
        auto mapping = agl::map<agl::Vec3>(b, GL_WRITE_ONLY);
        auto idx = 0;
        for(auto& v : mesh.vertices) {
            mapping[idx++] = v.normal;
        }
        agl::unmap(b);
        g.attributes["normal3"] = AttributeBuffer{
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(12),
            .type = GL_FLOAT,
        };
    }
    { // Positions.
        auto b = agl::buffer();
        agl::storage(b, size(mesh.vertices) * sizeof(agl::Vec3), GL_MAP_WRITE_BIT);
        auto mapping = agl::map<agl::Vec3>(b, GL_WRITE_ONLY);
        auto idx = 0;
        for(auto& v : mesh.vertices) {
            mapping[idx++] = v.position;
        }
        agl::unmap(b);
        g.attributes["position3"] = AttributeBuffer{
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(12),
            .type = GL_FLOAT,
        };
    }
    { // Texcoords.
        auto b = agl::buffer();
        agl::storage(b, size(mesh.vertices) * sizeof(agl::Vec3), GL_MAP_WRITE_BIT);
        auto mapping = agl::map<agl::Vec2>(b, GL_WRITE_ONLY);
        auto idx = 0;
        for(auto& v : mesh.vertices) {
            mapping[idx++] = v.texcoords;
        }
        agl::unmap(b);
        g.attributes["texcoords2"] = AttributeBuffer{
            .buffer = std::move(b),

            .size = agl::Size<GLint>(2),
            .stride = agl::Stride<GLsizei>(8),
            .type = GL_FLOAT,
        };
    }
    return g;
}

}

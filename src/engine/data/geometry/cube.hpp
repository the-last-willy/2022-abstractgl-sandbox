#pragma once

#include "dependencies.hpp"

#include <array>

constexpr auto cube_indexes = std::array<GLuint, 36>{
    // -x
    0 + 0, 0 + 2, 0 + 1,
    0 + 3, 0 + 1, 0 + 2,
    // +x
    4 + 0, 4 + 1, 4 + 2,
    4 + 3, 4 + 2, 4 + 1,
    // -y
    8 + 0, 8 + 1, 8 + 2,
    8 + 3, 8 + 2, 8 + 1,
    // +y
    12 + 0, 12 + 2, 12 + 1,
    12 + 3, 12 + 1, 12 + 2,
    // -z
    16 + 0, 16 + 2, 16 + 1,
    16 + 3, 16 + 1, 16 + 2,
    // +z
    20 + 0, 20 + 1, 20 + 2,
    20 + 3, 20 + 2, 20 + 1,
};

constexpr auto cube_normals = std::array{
    // -x
    agl::vec3(-1, 0, 0),
    agl::vec3(-1, 0, 0),
    agl::vec3(-1, 0, 0),
    agl::vec3(-1, 0, 0),
    // +x
    agl::vec3(+1, 0, 0),
    agl::vec3(+1, 0, 0),
    agl::vec3(+1, 0, 0),
    agl::vec3(+1, 0, 0),
    // -y
    agl::vec3(0, -1, 0),
    agl::vec3(0, -1, 0),
    agl::vec3(0, -1, 0),
    agl::vec3(0, -1, 0),
    // +y
    agl::vec3(0, +1, 0),
    agl::vec3(0, +1, 0),
    agl::vec3(0, +1, 0),
    agl::vec3(0, +1, 0),
    // -z
    agl::vec3(0, 0, -1),
    agl::vec3(0, 0, -1),
    agl::vec3(0, 0, -1),
    agl::vec3(0, 0, -1),
    // +z
    agl::vec3(0, 0, +1),
    agl::vec3(0, 0, +1),
    agl::vec3(0, 0, +1),
    agl::vec3(0, 0, +1),
};

constexpr auto cube_positions = std::array{
    // -x
    agl::vec3(-1, -1, -1),
    agl::vec3(-1, -1,  1),
    agl::vec3(-1,  1, -1),
    agl::vec3(-1,  1,  1),
    // +x
    agl::vec3(1, -1, -1),
    agl::vec3(1, -1, 1),
    agl::vec3(1, 1, -1),
    agl::vec3(1, 1, 1),
    // -y
    agl::vec3(-1, -1, -1),
    agl::vec3(-1, -1,  1),
    agl::vec3( 1, -1, -1),
    agl::vec3( 1, -1,  1),
    // +y
    agl::vec3(-1, 1, -1),
    agl::vec3(-1, 1, 1),
    agl::vec3( 1, 1, -1),
    agl::vec3( 1, 1, 1),
    // -z
    agl::vec3(-1, -1, -1),
    agl::vec3(-1,  1, -1),
    agl::vec3( 1, -1, -1),
    agl::vec3( 1,  1, -1),
    // +z
    agl::vec3(-1, -1, 1),
    agl::vec3(-1,  1, 1),
    agl::vec3( 1, -1, 1),
    agl::vec3( 1,  1, 1)
};

// const auto cube_texcoords = [](const auto& positions) {
//     auto texcoords = positions;
//     for(auto& v3 : texcoords) {
//         v3 = agl::normalize(v3);
//     }
//     return texcoords;
// }(cube_positions);

constexpr auto cube_texcoords = std::array{
    // -x
    agl::vec2(3.f / 4.f, 1.f / 3.f),
    agl::vec2(2.f / 4.f, 1.f / 3.f),
    agl::vec2(3.f / 4.f, 2.f / 3.f),
    agl::vec2(2.f / 4.f, 2.f / 3.f),
    // +x
    agl::vec2(0.f / 4.f, 1.f / 3.f),
    agl::vec2(1.f / 4.f, 1.f / 3.f),
    agl::vec2(0.f / 4.f, 2.f / 3.f),
    agl::vec2(1.f / 4.f, 2.f / 3.f),
    // -y
    agl::vec2(2.f / 4.f, 0.f / 3.f),
    agl::vec2(2.f / 4.f, 1.f / 3.f),
    agl::vec2(1.f / 4.f, 0.f / 3.f),
    agl::vec2(1.f / 4.f, 1.f / 3.f),
    // +y
    agl::vec2(2.f / 4.f, 3.f / 3.f),
    agl::vec2(2.f / 4.f, 2.f / 3.f),
    agl::vec2(1.f / 4.f, 3.f / 3.f),
    agl::vec2(1.f / 4.f, 2.f / 3.f),
    // -z
    agl::vec2(3.f / 4.f, 1.f / 3.f),
    agl::vec2(3.f / 4.f, 2.f / 3.f),
    agl::vec2(4.f / 4.f, 1.f / 3.f),
    agl::vec2(4.f / 4.f, 2.f / 3.f),
    // +z
    agl::vec2(2.f / 4.f, 1.f / 3.f),
    agl::vec2(2.f / 4.f, 2.f / 3.f),
    agl::vec2(1.f / 4.f, 1.f / 3.f),
    agl::vec2(1.f / 4.f, 2.f / 3.f)
};

inline
auto make_cube_geometry() {
    auto geometry = Geometry();
    geometry.primitive_count = agl::Count<GLsizei>(36);
    { // Indices.
        auto b = agl::buffer();
        agl::storage(b, std::span(cube_indexes));
        geometry.indexes = std::move(b);
    }
    { // 'normal3'
        auto b = agl::buffer();  
        agl::storage(b, std::span(cube_normals));
        geometry.attributes["normal3"] = {
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    { // 'position3'
        auto b = agl::buffer();  
        agl::storage(b, std::span(cube_positions));
        geometry.attributes["position3"] = {
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    return geometry;
}

inline
auto make_subdivided_cube_geometry(int n) {
    auto geometry = Geometry();
    geometry.primitive_count = agl::Count<GLsizei>(
        3 * (n + 1) * (n + 1) * 2 * 6);
    { // Indices.
        auto indices = std::vector<GLuint>();
        indices.reserve(3 * (n + 1) * (n + 1) * 2 * 6);
        for(int t = 0; t < 2 * 6; ++t) {
            auto offset = (n * (n + 5) / 2 + 3) * t;
            auto i_shift = [n](auto x) {
                return x * (2 * n - x + 5) / 2;
            };
            for(int i = 0; i < n + 1; ++i)
            for(int j = 0; j < n - i + 1; ++j) {
                indices.push_back(offset + i_shift(i + 0) + j + 0);
                indices.push_back(offset + i_shift(i + 1) + j + 0);
                indices.push_back(offset + i_shift(i + 0) + j + 1);
            }
            for(int i = 0; i < n; ++i)
            for(int j = 1; j < n - i + 1; ++j) {
                indices.push_back(offset + i_shift(i + 0) + j + 0);
                indices.push_back(offset + i_shift(i + 1) + j - 1);
                indices.push_back(offset + i_shift(i + 1) + j + 0);
            }
        }
        while(size(indices) != indices.capacity()) {
            indices.push_back(0);
        }
        auto b = agl::buffer();
        agl::storage(b, std::span(indices));
        geometry.indexes = std::move(b);
    }
    { // 'normal3'
        auto normals = std::vector<agl::Vec3>();
        normals.reserve((n * (n + 5) / 2 + 3) * 2 * 6);
        for(int t = 0; t < 2 * 6; ++t) {
            auto normal = cube_normals[2 * t];
            for(int i = 0; i < (n * (n + 5) / 2 + 3); ++i) {
                normals.push_back(normal);
            }
        }
        auto b = agl::buffer();  
        agl::storage(b, std::span(normals));
        geometry.attributes["normal3"] = {
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    { // 'position3'
        auto positions = std::vector<agl::Vec3>();
        positions.reserve((n * (n + 5) / 2 + 3) * 2 * 6);
        for(int t = 0; t < 2 * 6; ++t) {
            auto offset = 3 * t;
            auto origin = cube_positions[cube_indexes[offset + 0]];
            auto vi = (cube_positions[cube_indexes[offset + 1]] - origin) / float(n + 1);
            auto vj = (cube_positions[cube_indexes[offset + 2]] - origin) / float(n + 1);
            for(int i = 0; i < n + 2; ++i)
            for(int j = 0; j < n - i + 2; ++j) {
                auto p = origin + float(i) * vi + float(j) * vj;
                positions.push_back(origin + float(i) * vi + float(j) * vj);
            }
        }
        for(auto& p : positions) {
            p = agl::normalize(p);
        }
        auto b = agl::buffer();  
        agl::storage(b, std::span(positions));
        geometry.attributes["position3"] = {
            .buffer = std::move(b),

            .size = agl::Size<GLint>(3),
            .stride = agl::Stride<GLsizei>(3 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    { // 'texcoords3'
        auto texcoords = std::vector<agl::Vec2>();
        texcoords.reserve((n * (n + 5) / 2 + 3) * 2 * 6);
        for(int t = 0; t < 2 * 6; ++t) {
            auto offset = 3 * t;
            auto a = cube_texcoords[cube_indexes[offset + 0]];
            auto b = cube_texcoords[cube_indexes[offset + 1]];
            auto c = cube_texcoords[cube_indexes[offset + 2]];
            for(int i = 0; i < n + 2; ++i)
            for(int j = 0; j < n - i + 2; ++j) {
                auto tb = float(i) / (n + 1);
                auto tc = float(j) / (n + 1);
                auto barycentric = (1.f - tb - tc) * a + tb * b + tc * c;
                texcoords.push_back(barycentric);
            }
        }
        auto b = agl::buffer();  
        agl::storage(b, std::span(texcoords));
        geometry.attributes["texcoords2"] = {
            .buffer = std::move(b),

            .size = agl::Size<GLint>(2),
            .stride = agl::Stride<GLsizei>(2 * sizeof(GLfloat)),
            .type = GL_FLOAT
        };
    }
    return geometry;
}

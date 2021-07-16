#pragma once

#include "gl/matrix/all.hpp"
#include "gl/vector/all.hpp"

namespace tlw {
namespace scene {

struct Camera {
    Vec3 position = {0.f, 0.f, 0.f};
    float offset = -2.f;
    
    // Aircraft rotation.
    float yaw = 0.f;
    float pitch = 0.f;

    float near = 0.1f;
    float far = 1000.f;

    float aspect_ratio = 16.f / 9.f;
    // float fov = 90.f;
};

constexpr
auto orthographic(const Camera& c) {
    // View space -> Clip space
    // (near, far) -> (-1, 1) 

    // | 1 | 0 | 0 | 0 |
    // | 0 | 1 | 0 | 0 |
    // | 0 | 0 | a | b |
    // | 0 | 0 | 0 | 1 |
    
    auto a = 2 / (c.far - c.near);
    auto b = -(c.far + c.near) / (c.far - c.near);

    auto y = c.aspect_ratio;
    return Mat4{ // A line is a column.
        1.f, 0.f, 0.f, 0.f, 
        0.f,   y, 0.f, 0.f,
        0.f, 0.f,   a, 0.f,
        0.f, 0.f,   b, 1.f};
}

constexpr
auto perspective(const Camera& c) {
    // View space -> Clip space
    // (near, far) -> (-near, far) 

    // | 1 | 0 | 0 | 0 | = x
    // | 0 | 1 | 0 | 0 | = y
    // | 0 | 0 | a | b | -> (-near, far)
    // | 0 | 0 | 1 | 0 | = z
    
    auto a = (c.far + c.near) / (c.far - c.near);
    auto b = -2 * c.far * c.near / (c.far - c.near);

    auto y = c.aspect_ratio;
    return Mat4{ // A line is a column.
        1.f, 0.f,  0.f, 0.f, 
        0.f,   y,  0.f, 0.f,
        0.f, 0.f,    a, 1.f,
        0.f, 0.f,    b, 0.f};
}

constexpr
auto rotation(const Camera& c) {
    return rotation_y(c.yaw)
    * rotation_x(c.pitch);
}

constexpr
auto transformation(const Camera& c) {
    return translation(Vec3{0.f, 0.f, c.offset})
    * rotation_x(-c.pitch)
    * rotation_y(-c.yaw)
    * translation(-c.position);
}

}}

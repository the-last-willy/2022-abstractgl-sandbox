#pragma once

#include <agl/all.hpp>

namespace mesh {

struct Vertex {
    agl::Vec3 color;
    agl::Vec3 normal;
    agl::Vec3 position;
    agl::Vec2 texcoords;
};

inline
auto barycentric_interpolation(
    const Vertex& a,
    const Vertex& b,
    const Vertex& c,
    float tb, float tc)
{
    auto v = Vertex();
    auto ta = 1.f - tb - tc;
    v.color = ta * a.color + tb * b.color + tc * c.color;
    v.normal = ta * a.normal + tb * b.normal + tc * c.normal;
    v.position = ta * a.position + tb * b.position + tc * c.position;
    v.texcoords = ta * a.texcoords + tb * b.texcoords + tc * c.texcoords;
    return v;
}

}

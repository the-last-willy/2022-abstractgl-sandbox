#pragma once

#include "uniform.hpp"

#include <agl/all.hpp>

#include <deque>
#include <map>
#include <optional>
#include <vector>

namespace eng {

struct Primitive {
    agl::DrawMode draw_mode = agl::DrawMode::triangles;
    agl::DrawType draw_type;
    // std::size_t material = 0;
    agl::Count<GLsizei> primitive_count = agl::Count<GLsizei>(0);
    agl::VertexArray vertex_array = {};
};

struct Database {
    // OpenGL resources.

    // Used to configure the vertex arrays.

    // Keep track of buffers until they can be deleted.
    // (reference counting ?)
    std::map<std::size_t, agl::Buffer> gl_buffers = {};

    std::map<std::size_t, agl::Program> gl_programs = {};

    // std::vector<agl::Sampler> sampler = {};

    std::map<std::size_t, agl::Texture> gl_textures = {};

    // Need to be bound before rendering.
    // Need to know number of primitives.
    // Need to know if index buffer and index type.
    std::map<std::size_t, agl::VertexArray> gl_vertex_arrays = {};

    ////////////////////////////////////////////////////////////////////////////
    // Objects.

    // Material:
        // Texture

    // Mesh:
        // Primitive[]


    std::deque<Primitive> primitives = {};
    // Primitive:
        // agl::DrawMode
        // agl::VertexArray
        // Material

    // Texture:
        // agl::Sampler
        // agl::Texture

    ////////////////////////////////////////////////////////////////////////////
    // Uniforms.

    // std::map<MeshId, std::map<std::string, AnyUniform>> material_uniforms = {};

    // std::map<MeshId, std::map<std::string, AnyUniform>> mesh_uniforms = {};
};

}

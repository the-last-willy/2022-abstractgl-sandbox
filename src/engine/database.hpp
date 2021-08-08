#pragma once

#include "material.hpp"
#include "primitive.hpp"
#include "uniform.hpp"

#include <agl/all.hpp>

#include <deque>
#include <map>
#include <optional>
#include <set>
#include <vector>

namespace eng {

struct Database {
    // OpenGL resources.

    // Used to configure the vertex arrays.

    // Keep track of buffers until they can be deleted.
    // (reference counting ?)
    std::map<std::size_t, agl::Buffer> gl_buffers = {};

    std::map<std::size_t, agl::Program> gl_programs = {};

    // std::vector<agl::Sampler> sampler = {};

    std::deque<agl::Texture> gl_textures = {};

    // Need to be bound before rendering.
    // Need to know number of primitives.
    // Need to know if index buffer and index type.
    std::map<std::size_t, agl::VertexArray> gl_vertex_arrays = {};

    ////////////////////////////////////////////////////////////////////////////
    // Objects.

    std::deque<Material> materials = {};
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

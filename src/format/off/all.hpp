#pragma once

#include <agl/all.hpp>
#include <file.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace format::off {

struct File {
    std::vector<agl::Uvec3> triangle_indices = {};
    std::vector<agl::Vec3> vertex_positions = {};
};

inline
auto read(const std::string& filepath) {
    auto f = File();

    auto stream = std::stringstream(file(filepath));

    std::size_t vertex_count;
    stream >> vertex_count;
    f.vertex_positions.resize(vertex_count);

    std::size_t face_count;
    stream >> face_count;
    f.triangle_indices.resize(face_count);
    
    std::size_t edge_count;
    stream >> edge_count;

    for(std::size_t i = 0; i < vertex_count; ++i) {
        auto& vp = f.vertex_positions[i];
        stream >> vp[0] >> vp[1] >> vp[2];
    }

    for(std::size_t i = 0; i < face_count; ++i) {
        auto& ti = f.triangle_indices[i];
        stream >> ti[0] >> ti[1] >> ti[2];
    }

    return f;
}

};

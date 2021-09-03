#pragma once

#include "dependencies.hpp"

namespace gltf {

inline
auto cubic_shadow_mapping_material() {
    auto m = eng::Material();
    load(m.program, {
        {
            agl::vertex_shader_tag,
            file(tlw::root + "src/shader/gltf/cube_face_depth.vs")
        },
        {
            agl::fragment_shader_tag,
            file(tlw::root + "src/shader/gltf/cube_face_depth.fs")
        }});
    m.program.capabilities.emplace_back(
        agl::Capability::cull_face, []() {
            glCullFace(GL_FRONT); });
    m.program.capabilities.emplace_back(
        agl::Capability::depth_test, []() {
            glDepthFunc(GL_LESS); });
    return m;
}

}

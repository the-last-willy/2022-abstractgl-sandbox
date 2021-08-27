#pragma once

#include "engine/data/gltf/cubic_shadow_mapping_material.hpp"
#include "engine/scene/perspective_projection.hpp"
#include "engine/scene/view.hpp"
#include "engine/framebuffer.hpp"
#include "engine/material.hpp"
#include "engine/texture.hpp"

#include <array>
#include <memory>

namespace eng {

struct OmniShadowMap {
    // Transforms.

    agl::Vec3 position = {};
    tlw::PerspectiveProjection projection = {};
    std::array<tlw::View, 6> views = {};

    // OpenGL stuff.

    Framebuffer framebuffer = agl::none;
    Material material = {};
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();

    OmniShadowMap(GLsizei resolution) {
        { // Projection.
            projection.aspect_ratio = 1.f;
        }
        { // Views.
            views[0].yaw = agl::pi / 2.f; // X+
            views[1].yaw = -agl::pi / 2.f; // X-
            views[2].pitch = agl::pi / 2.f; // Y+
            views[3].pitch = -agl::pi / 2.f; // Y-
            views[4].yaw = 0.f; // Z+
            views[5].yaw = agl::pi; // Z-
        }

        { // Framebuffer.
            framebuffer.opengl = create(agl::framebuffer_tag);
            framebuffer.on_bind = [resolution](const agl::Framebuffer&) {
                glViewport(
                    0, 0,
                    static_cast<int>(resolution),
                    static_cast<int>(resolution)); };
            framebuffer.on_clear = [](const agl::Framebuffer& f) {
                clear(f, agl::depth_tag, 1.f); };
        }
        { // Material.
            material = gltf::cubic_shadow_mapping_material();
        }
        { // Texture.
            auto& t = texture->texture = create(agl::TextureTarget::cube_map_array);
            mag_filter(t, GL_NEAREST);
            min_filter(t, GL_NEAREST);
            parameter(t, agl::TextureParameter::wrap_s, GL_CLAMP_TO_EDGE);
            parameter(t, agl::TextureParameter::wrap_t, GL_CLAMP_TO_EDGE);
            storage(
                t, 1, GL_DEPTH_COMPONENT16,
                agl::Width(resolution),
                agl::Height(resolution),
                agl::Depth(6));
        }
    }
};

inline
void bind(const OmniShadowMap& osm) {
    bind(osm.framebuffer);
    bind(osm.material);
}

inline
void unbind(const OmniShadowMap& osm) {
    unbind(osm.framebuffer);
    unbind(osm.material);
}

}

#pragma once

#include "material.hpp"
#include "texture.hpp"

#include <agl/all.hpp>

#include <map>
#include <memory>
#include <functional>

namespace eng {

struct Framebuffer {
    agl::Framebuffer opengl = agl::none;

    std::map<std::string, std::shared_ptr<Texture>> color_attachments = {};

    std::function<void(const agl::Framebuffer&)> on_clear
    = [](const agl::Framebuffer&) {};
};

inline
void bind(const Framebuffer& f) {
    agl::bind(f.opengl);
}

void unbind(const Framebuffer&) {
    agl::bind(agl::default_framebuffer);
}

inline
void clear(const Framebuffer& f) {
    f.on_clear(f.opengl);
}

inline
Framebuffer gbuffer(int width, int height) {
    auto fb = Framebuffer();
    fb.opengl = agl::create(agl::framebuffer_tag);
    auto default_sampler = create(agl::sampler_tag);
    { // Sampler.
        mag_filter(default_sampler, GL_LINEAR);
        min_filter(default_sampler, GL_LINEAR);
    }
    { // Albedo texture.
        auto& tex = *(fb.color_attachments["albedo_texture"]
            = std::make_shared<eng::Texture>());
        tex.sampler = default_sampler;
        tex.texture = agl::create(agl::TextureTarget::_2d);
        storage(
            tex.texture, GL_R11F_G11F_B10F,
            agl::Width(width), agl::Height(height));
    }
    
    { // Emissive texture.
        auto& tex = *(fb.color_attachments["emissive_texture"]
            = std::make_shared<eng::Texture>());
        tex.sampler = default_sampler;
        tex.texture = agl::create(agl::TextureTarget::_2d);
        storage(
            tex.texture, GL_R11F_G11F_B10F,
            agl::Width(width), agl::Height(height));
    }
    { // Metallic roughness texture.
        auto& tex = *(fb.color_attachments["metallic_roughness_texture"]
            = std::make_shared<eng::Texture>());
        tex.sampler = default_sampler;
        tex.texture = agl::create(agl::TextureTarget::_2d);
        storage(
            tex.texture, GL_R11F_G11F_B10F,
            agl::Width(width), agl::Height(height));
    }
    { // Normal texture.
        auto& tex = *(fb.color_attachments["normal_texture"]
            = std::make_shared<eng::Texture>());
        tex.sampler = default_sampler;
        tex.texture = agl::create(agl::TextureTarget::_2d);
        storage(
            tex.texture, GL_RGB32F,
            agl::Width(width), agl::Height(height));
    }
    { // Occlusion texture. 
        auto& tex = *(fb.color_attachments["occlusion_texture"]
            = std::make_shared<eng::Texture>());
        tex.sampler = default_sampler;
        tex.texture = agl::create(agl::TextureTarget::_2d);
        storage(
            tex.texture, GL_RGB32F,
            agl::Width(width), agl::Height(height));
    }
    { // Position texture. 
        auto& tex = *(fb.color_attachments["position_texture"]
            = std::make_shared<eng::Texture>());
        tex.sampler = default_sampler;
        tex.texture = agl::create(agl::TextureTarget::_2d);
        storage(
            tex.texture, GL_RGB32F,
            agl::Width(width), agl::Height(height));
    }
    return fb;
}

}

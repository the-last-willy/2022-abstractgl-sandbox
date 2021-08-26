#pragma once

#include <common/all.hpp>
#include <engine/all.hpp>

#include <tiny_gltf.h>

#include <algorithm>
#include <map>
#include <memory>

namespace format::gltf2 {

struct Content {
    std::map<int, eng::Accessor> accessors = {};
    std::map<int, std::shared_ptr<eng::Animation>> animations = {};
    std::map<int, std::shared_ptr<eng::Buffer>> buffers = {};
    std::map<int, std::shared_ptr<eng::Camera>> cameras = {};
    std::map<int, agl::Texture> images = {};
    std::map<int, std::shared_ptr<eng::Material>> materials = {};
    std::map<int, std::shared_ptr<eng::Mesh>> meshes = {};
    std::map<int, std::shared_ptr<eng::Node>> nodes = {};
    std::map<int, agl::Sampler> samplers = {};
    std::map<int, std::shared_ptr<eng::Scene>> scenes = {};
    std::map<int, std::shared_ptr<eng::Texture>> textures = {};
};


// Requires accessors, nodes.
inline
void convert_animations(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.animations); ++i) {
        auto& animation = model.animations[i];
        auto& eng_animation = *(content.animations[static_cast<int>(i)]
            = std::make_shared<eng::Animation>());
        auto eng_samplers = std::map<int, std::shared_ptr<eng::AnimationSampler>>();
        for(std::size_t j = 0; j < size(animation.samplers); ++j) {
            auto& sampler = animation.samplers[j];
            auto& eng_sampler = *(eng_samplers[static_cast<int>(j)]
            = std::make_shared<eng::AnimationSampler>());
            eng_sampler.input = content.accessors.at(sampler.input);
            eng_sampler.output = content.accessors.at(sampler.output);
            if(sampler.interpolation == "CUBICSPLINE") {
                eng_sampler.interpolation = eng::AnimationSamplerInterpolation::cubic_spline;
            } else if(sampler.interpolation == "LINEAR") {
                eng_sampler.interpolation = eng::AnimationSamplerInterpolation::linear;
            } else if(sampler.interpolation == "STEP") {
                eng_sampler.interpolation = eng::AnimationSamplerInterpolation::step;
            }
        }
        eng_animation.channels.reserve(size(animation.channels));
        for(auto& channel : animation.channels) {
            auto& eng_channel = *eng_animation.channels.emplace_back(
                std::make_shared<eng::AnimationChannel>());
            eng_channel.target_node = content.nodes.at(channel.target_node);
            eng_channel.sampler = eng_samplers.at(channel.sampler);
            if       (channel.target_path == "rotation") {
                eng_channel.target_path = eng::AnimationTargetPath::rotation;
            } else if(channel.target_path == "scale") {
                eng_channel.target_path = eng::AnimationTargetPath::scale;
            } else if(channel.target_path == "translation") {
                eng_channel.target_path = eng::AnimationTargetPath::translation;
            } else if(channel.target_path == "weights") {
                eng_channel.target_path = eng::AnimationTargetPath::weights;
            }
        }
    }
}

// Requires buffers.
inline
void convert_accessors(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.accessors); ++i) {
        auto& accessor = model.accessors[i];
        auto& eng_accessor = content.accessors[static_cast<int>(i)] 
        = eng::Accessor();
        eng_accessor.count = accessor.count;
        eng_accessor.component_type = accessor.componentType;
        eng_accessor.byte_offset = agl::Offset<GLuint>(static_cast<GLuint>(accessor.byteOffset));
        eng_accessor.normalized = agl::Normalized(accessor.normalized);
        switch(accessor.type) {
        case TINYGLTF_TYPE_SCALAR:
            eng_accessor.component_count = agl::Size<GLint>(1);
            break;
        case TINYGLTF_TYPE_VEC2:
            eng_accessor.component_count = agl::Size<GLint>(2);
            break;
        case TINYGLTF_TYPE_VEC3:
            eng_accessor.component_count = agl::Size<GLint>(3);
            break;
        case TINYGLTF_TYPE_VEC4:
            eng_accessor.component_count = agl::Size<GLint>(4);
            break;
        default:
            throw std::runtime_error("Unhandled accessor type.");
            break;
        }
        auto& buffer_view = model.bufferViews[accessor.bufferView];
        eng_accessor.buffer_view_byte_offset = agl::offset<GLintptr>(buffer_view.byteOffset);
        eng_accessor.buffer_view_byte_stride = agl::stride<GLsizei>(buffer_view.byteStride);
        if(eng_accessor.buffer_view_byte_stride.value == 0) {
            switch(eng_accessor.component_type) {
            case GL_FLOAT:
                eng_accessor.component_size = 4;
                break;
            case GL_UNSIGNED_BYTE:
                eng_accessor.component_size = 1;
                break;
            case GL_UNSIGNED_SHORT:
                eng_accessor.component_size = 2;
                break;
            default:
                throw std::runtime_error("Unhandled component type.");
                break;
            }
            eng_accessor.buffer_view_byte_stride
            = agl::Stride<GLsizei>(
                eng_accessor.component_count.value
                * eng_accessor.component_size);
        }
        eng_accessor.buffer = content.buffers.at(buffer_view.buffer);
    }
}

inline
void convert_buffers(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.buffers); ++i) {
        auto& buffer = model.buffers[i];
        auto& eng_buffer = *(content.buffers[static_cast<int>(i)]
            = std::make_shared<eng::Buffer>());
        auto copy = std::back_inserter(eng_buffer.data);
        for(auto d : buffer.data) {
            *copy++ = static_cast<std::byte>(d);
        }
        eng_buffer.opengl = agl::create(agl::buffer_tag);
        agl::storage(eng_buffer.opengl, std::span(buffer.data));
    }
}

inline
void convert_cameras(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.cameras); ++i) {
        auto& camera = model.cameras[i];
        auto& eng_camera = *(content.cameras[static_cast<int>(i)]
            = std::make_shared<eng::Camera>());
        if(camera.type == "orthographic") {
            throw std::runtime_error("Not implemented camera type.");
        } else if(camera.type == "perspective") {
            auto& persp = camera.perspective;
            auto& pp = eng_camera.projection.emplace<eng::PerspectiveProjection>();
            pp.aspect_ratio = static_cast<float>(persp.aspectRatio);
            pp.y_fov = static_cast<float>(persp.yfov);
            pp.z_near = static_cast<float>(persp.znear);
            if(persp.zfar > 0.f) {
                pp.z_far = static_cast<float>(persp.zfar);
            }
        } else {
            throw std::runtime_error("Unhandled camera types.");
        }
    }
}

inline
void convert_images(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.images); ++i) {
        auto& image = model.images[i];
        auto& t = content.images[static_cast<int>(i)]
        = agl::create(agl::TextureTarget::_2d);
        GLenum base_internal_format = 0;
        GLenum pixel_data_type = 0;
        GLenum sized_internal_format = 0;
        if(image.component == 3 && image.bits == 8) {
            base_internal_format = GL_RGB;
            pixel_data_type = GL_UNSIGNED_BYTE;
            sized_internal_format = GL_RGB8;
        } else if(image.component == 4 && image.bits == 8) {
            base_internal_format = GL_RGBA;
            pixel_data_type = GL_UNSIGNED_BYTE;
            sized_internal_format = GL_RGBA8;
        } else {
            throw std::runtime_error("Invalid texture format.");
        }
        auto level_count = static_cast<GLsizei>(
            std::floor(std::log2(std::max(image.height, image.width))) + 1);
        agl::storage(
                t, level_count, sized_internal_format,
                agl::Width(image.width), agl::Height(image.height));
        agl::image(
            t, agl::Width(image.width), agl::Height(image.height),
            base_internal_format, pixel_data_type,
            std::as_bytes(std::span(image.image)));
        agl::generate_mipmap(t);
        // glFlush(); // Fixed a bug at some point.
    }
}

inline
void convert_materials(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.materials); ++i) {
        auto& material = model.materials[i];
        auto& eng_material = *(content.materials[static_cast<int>(i)]
            = std::make_shared<eng::Material>());
        eng_material.program.capabilities.emplace_back(
            agl::Capability::depth_test, []() {
                glDepthFunc(GL_LESS); });
        { // 'doubleSided'.
            if(!material.doubleSided) {
                eng_material.program.capabilities.push_back({
                    agl::Capability::cull_face, []() {
                        glCullFace(GL_FRONT); }});
            }
        }
        { // 'emissiveFactor'.
            auto& ef = material.emissiveFactor;  
            if(size(ef) == 3) {
                eng_material.uniforms["emissiveFactor"]
                = new eng::Uniform<agl::Vec3>(agl::vec3(
                    static_cast<float>(ef[0]),
                    static_cast<float>(ef[1]),
                    static_cast<float>(ef[2])));
            }
        }
        { // 'emissiveTexture'.
            auto& et = material.emissiveTexture;  
            if(et.index != -1) {
                eng_material.textures["emissiveTexture"]
                = content.textures.at(et.index);
            }
        }
        { // 'normalTexture'.
            auto& normalTexture = material.normalTexture;  
            if(normalTexture.index != -1) {
                eng_material.textures["normalTexture"]
                = content.textures.at(normalTexture.index);
            }
        }
        { // 'occlusionTexture'.
            auto& t = material.occlusionTexture;  
            if(t.index != -1) {
                eng_material.textures["occlusionTexture"]
                = content.textures.at(t.index);
                eng_material.uniforms["occlusionStrength"]
                = new eng::Uniform<GLfloat>(
                    static_cast<float>(t.strength));
            }
        }
        { // 'pbrMetallicRoughness'.
            auto& pmr = material.pbrMetallicRoughness;
            { // 'baseColorFactor'.
                auto& bcf = pmr.baseColorFactor;
                eng_material.uniforms["baseColorFactor"]
                = new eng::Uniform<agl::Vec4>(agl::vec4(
                    static_cast<float>(bcf[0]),
                    static_cast<float>(bcf[1]),
                    static_cast<float>(bcf[2]),
                    static_cast<float>(bcf[3])));
            } 
            { // 'baseColorTexture'.
                auto& baseColorTexture = pmr.baseColorTexture;
                if(baseColorTexture.index != -1) {
                    eng_material.textures["baseColorTexture"]
                    = content.textures.at(baseColorTexture.index);
                }
            }
            { // 'metallicFactor'.
                eng_material.uniforms["metallicFactor"]
                = new eng::Uniform<GLfloat>(
                    static_cast<GLfloat>(pmr.metallicFactor));
            }
            { // 'metallicRoughnessTexture'.
                auto& mrt = pmr.metallicRoughnessTexture;
                if(mrt.index != -1) {
                    eng_material.textures["metallicRoughnessTexture"]
                    = content.textures.at(mrt.index);
                }
            }
            { // 'roughnessFactor'.
                eng_material.uniforms["roughnessFactor"]
                = new eng::Uniform<GLfloat>(
                    static_cast<GLfloat>(pmr.roughnessFactor));
            }
        }
    }
}

inline
void convert_meshes(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.meshes); ++i) {
        auto& mesh = model.meshes[i];
        auto& eng_mesh = *(content.meshes[static_cast<int>(i)]
            = std::make_shared<eng::Mesh>());
        for(auto& primitive : mesh.primitives) {
            auto eng_primitive = eng::Primitive();
            {
                eng_primitive.draw_mode = static_cast<agl::DrawMode>(primitive.mode);
            }
            auto gl_vertex_array = eng_primitive.vertex_array = agl::vertex_array();

            { // Indices.
                auto& accessor = model.accessors[primitive.indices];
                auto& buffer_view = model.bufferViews[accessor.bufferView];
                if(accessor.componentType == GL_UNSIGNED_BYTE) {
                    auto& buffer = model.buffers[buffer_view.buffer];
                    auto short_data = std::vector<GLushort>(accessor.count);
                    auto offset = accessor.byteOffset + buffer_view.byteOffset;
                    for(std::size_t j = 0; j < size(short_data); ++j) {
                        short_data[j] = buffer.data[offset + j];
                    }
                    auto eng_buffer = agl::create(agl::buffer_tag);
                    storage(eng_buffer, std::span(short_data));
                    eng_primitive.draw_type = static_cast<agl::DrawType>(GL_UNSIGNED_SHORT);
                    eng_primitive.offset = 0;
                    eng_primitive.primitive_count = agl::Count<GLsizei>(
                        static_cast<GLsizei>(accessor.count));
                    agl::element_buffer(
                        gl_vertex_array,
                        eng_buffer);
                } else {
                    eng_primitive.draw_type = static_cast<agl::DrawType>(accessor.componentType);
                    eng_primitive.offset = accessor.byteOffset + buffer_view.byteOffset;
                    eng_primitive.primitive_count = agl::Count<GLsizei>(
                        static_cast<GLsizei>(accessor.count));
                    agl::element_buffer(
                        gl_vertex_array,
                        content.buffers.at(buffer_view.buffer)->opengl);
                }
            }
            if(primitive.material != -1) {
                eng_primitive.material = content.materials.at(primitive.material);
            }
            for(auto [name, accessor_id] : primitive.attributes) {
                eng_primitive.attributes[name] = content.accessors.at(accessor_id);
            }
            eng_mesh.primitives.emplace_back(
                std::make_shared<eng::Primitive>(eng_primitive));
        }
    }
}

inline
void convert_nodes(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.nodes); ++i) {
        content.nodes[static_cast<int>(i)] = std::make_shared<eng::Node>();
    }
    for(std::size_t i = 0; i < size(model.nodes); ++i) {
        auto& node = model.nodes[i];
        auto& eng_node = *content.nodes[static_cast<int>(i)];
        for(auto c : node.children) {
            eng_node.children.push_back(content.nodes.at(c));
        }
        if(size(node.matrix) == 16) {
            auto m = [mat = node.matrix](std::size_t i) {
                return static_cast<float>(mat[i]); }; 
            eng_node.transform = eng_node.transform * agl::mat4(
                m(0), m(1), m(2), m(3), 
                m(4), m(5), m(6), m(7), 
                m(8), m(9), m(10), m(11), 
                m(12), m(13), m(14), m(15));
        } else {
            if(size(node.translation) == 3) {
                eng_node.transform = eng_node.transform * agl::translation(
                    static_cast<float>(node.translation[0]),
                    static_cast<float>(node.translation[1]),
                    static_cast<float>(node.translation[2]));
            }
            if(size(node.rotation) == 4) {
                auto a = static_cast<float>(node.rotation[3]);
                auto b = static_cast<float>(node.rotation[0]);
                auto c = static_cast<float>(node.rotation[1]);
                auto d = static_cast<float>(node.rotation[2]);
                
                auto aa = a * a;
                auto bb = b * b;
                auto cc = c * c;
                auto dd = d * d;
                
                auto ab = 2.f * a * b;
                auto ac = 2.f * a * c;
                auto ad = 2.f * a * d;
                auto bc = 2.f * b * c;
                auto bd = 2.f * b * d;
                auto cd = 2.f * c * d;

                eng_node.transform = eng_node.transform * agl::mat4(
                    aa + bb - cc - dd,           bc + ad,           bd - ac, 0.f,
                                bc - ad, aa - bb + cc - dd,           cd + ab, 0.f,
                                bd + ac,           cd - ab, aa - bb - cc + dd, 0.f,
                                    0.f,                             0.f, 0.f, 1.f);
            }
            if(size(node.scale) == 3) {
                eng_node.transform = eng_node.transform *  agl::scaling3(
                    static_cast<float>(node.scale[0]),
                    static_cast<float>(node.scale[1]),
                    static_cast<float>(node.scale[2]));
            }
        }
        if(node.camera != -1) {
            eng_node.camera = content.cameras.at(node.camera);
        }
        if(node.mesh != -1) {
            eng_node.mesh = content.meshes.at(node.mesh);
        }
    }
}

inline
void convert_samplers(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.samplers); ++i) {
        auto& sampler = model.samplers[i];
        auto& eng_sampler = content.samplers[static_cast<int>(i)]
        = create(agl::sampler_tag);
        if(sampler.magFilter != -1) {
            mag_filter(eng_sampler, sampler.magFilter);
        }
        if(sampler.minFilter != -1) {
            min_filter(eng_sampler, sampler.minFilter);
        }
        parameter(eng_sampler, agl::TextureParameter::wrap_s, sampler.wrapS);
        parameter(eng_sampler, agl::TextureParameter::wrap_t, sampler.wrapT);
    }
}

inline
void convert_scenes(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.scenes); ++i) {
        auto& scene = model.scenes[i];
        auto& eng_scene = *(content.scenes[static_cast<int>(i)]
            = std::make_shared<eng::Scene>());
        for(auto n : scene.nodes) {
            eng_scene.nodes.push_back(content.nodes.at(n));
        }
    }
}

inline
void convert_textures(Content& content, tinygltf::Model& model) {
    for(std::size_t i = 0; i < size(model.textures); ++i) {
        auto& texture = model.textures[i];
        auto& eng_texture = *(content.textures[static_cast<int>(i)]
            = std::make_shared<eng::Texture>());
        if(texture.sampler != -1) {
            eng_texture.sampler = content.samplers.at(texture.sampler);
        }
        eng_texture.texture = content.images.at(texture.source);
    }
}

inline
auto fill(tinygltf::Model& model) {
    auto content = Content();
    convert_buffers(content, model);
    convert_images(content, model);
    convert_samplers(content, model);
    convert_textures(content, model);
    convert_materials(content, model);
    convert_accessors(content, model);
    convert_meshes(content, model);
    convert_cameras(content, model);
    convert_nodes(content, model);
    convert_scenes(content, model);
    convert_animations(content, model);
    return content;
}

}

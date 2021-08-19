#pragma once

#include <common/all.hpp>
#include <engine/all.hpp>

#include <tiny_gltf.h>

#include <map>
#include <memory>

namespace format::gltf2 {

struct Content {
    std::map<int, eng::Accessor> accessors = {};
    std::map<int, agl::Buffer> buffers = {};
    std::map<int, agl::Texture> images = {};
    std::map<int, eng::Material> materials = {};
    std::map<int, std::shared_ptr<eng::Mesh>> meshes = {};
    std::map<int, std::shared_ptr<eng::Node>> nodes = {};
    std::map<int, std::shared_ptr<eng::Scene>> scenes = {};
    std::map<int, eng::Texture> textures = {};
};

inline
auto fill(tinygltf::Model& model) {
    auto content = Content();

    { // Converting buffers.
        for(std::size_t i = 0; i < size(model.buffers); ++i) {
            auto b = agl::create(agl::buffer_tag);
            agl::storage(b, std::span(model.buffers[i].data));

            content.buffers[static_cast<int>(i)] = std::move(b);
        }
    }

    { // Converting images into gl textures.
        for(std::size_t i = 0; i < size(model.images); ++i) {
            auto& image = model.images[i];
            auto t = agl::create(agl::TextureTarget::_2d);
            mag_filter(t, GL_LINEAR);
            min_filter(t, GL_LINEAR_MIPMAP_LINEAR);
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
            glFlush();

            content.images[static_cast<int>(i)] = std::move(t);
        }
    }

    { // Converting textures.
        for(std::size_t i = 0; i < size(model.textures); ++i) {
            auto& texture = model.textures[i];

            content.textures[static_cast<int>(i)] = {
                content.images.at(texture.source) };
        }
    }

    { // Converting materials.
        for(std::size_t i = 0; i < size(model.materials); ++i) {
            auto& material = model.materials[i];
            auto eng_material = eng::Material();

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
            { // 'emissiveTexture'.
                auto& et = material.emissiveTexture;  
                if(et.index != -1) {
                    eng_material.textures["emissiveTexture"]
                    = content.textures.at(et.index).texture;
                }
            }
            { // 'normalTexture'.
                auto& normalTexture = material.normalTexture;  
                if(normalTexture.index != -1) {
                    eng_material.textures["normalTexture"]
                    = content.textures.at(normalTexture.index).texture;
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
                        = content.textures.at(baseColorTexture.index).texture;
                    }
                }
                { // 'metallicRoughnessTexture'.
                    auto& mrt = pmr.metallicRoughnessTexture;
                    if(mrt.index != -1) {
                        eng_material.textures["metallicRoughnessTexture"]
                        = content.textures.at(mrt.index).texture;
                    }
                }
            }
            content.materials[static_cast<int>(i)] = std::move(eng_material);
        }
    }

    { // Converting accessors.
        for(std::size_t i = 0; i < size(model.accessors); ++i) {
            auto& accessor = model.accessors[i];
            auto eng_accessor = eng::Accessor();

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

            eng_accessor.buffer_view_byte_offset
            = agl::Offset<GLintptr>(static_cast<GLintptr>(buffer_view.byteOffset));
            eng_accessor.buffer_view_byte_stride
            = agl::Stride<GLsizei>(static_cast<GLsizei>(buffer_view.byteStride));

            if(eng_accessor.buffer_view_byte_stride.value == 0) {
                switch(eng_accessor.component_type) {
                case GL_FLOAT:
                    eng_accessor.component_size = 4;
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

            content.accessors[static_cast<int>(i)] = std::move(eng_accessor);
        }
    }
    { // Converting meshes and primitives.
        for(std::size_t i = 0; i < size(model.meshes); ++i) {
            auto& mesh = model.meshes[i];
            auto& eng_mesh
            = *(content.meshes[static_cast<int>(i)]
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
                    eng_primitive.draw_type = static_cast<agl::DrawType>(accessor.componentType);
                    eng_primitive.offset = buffer_view.byteOffset;
                    eng_primitive.primitive_count = agl::Count<GLsizei>(
                        static_cast<GLsizei>(accessor.count));
                    agl::element_buffer(
                        gl_vertex_array,
                        content.buffers.at(buffer_view.buffer));
                }

                eng_primitive.material = content.materials.at(primitive.material);


                for(auto [name, id] : primitive.attributes) {
                    eng_primitive.attributes[name] = content.accessors.at(id);
                }

                eng_mesh.primitives.emplace_back(
                    std::make_shared<eng::Primitive>(eng_primitive));
            }
        }
    }
    { // Converting nodes.
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
                std::cout << "Node matrix not implemented." << std::endl;
            } else {
                if(size(node.translation) == 3) {
                    eng_node.transform = eng_node.transform * agl::translation(
                        static_cast<float>(node.translation[0]),
                        static_cast<float>(node.translation[1]),
                        static_cast<float>(node.translation[2]));
                }
                if(size(node.scale) == 3) {
                    eng_node.transform = eng_node.transform *  agl::scaling3(
                        static_cast<float>(node.scale[0]),
                        static_cast<float>(node.scale[1]),
                        static_cast<float>(node.scale[2]));
                }

                if(size(node.rotation) > 0) {
                    std::cout << "Node rotation not implemented." << std::endl;
                }
            }

            if(node.mesh != -1) {
                eng_node.mesh = content.meshes.at(node.mesh);
            }

            
        }
    }
    { // Converting scenes.
        for(std::size_t i = 0; i < size(model.scenes); ++i) {
            auto& scene = model.scenes[i];

            auto& eng_scene
            = *(content.scenes[static_cast<int>(i)]
                = std::make_shared<eng::Scene>());
            
            for(auto n : scene.nodes) {
                eng_scene.nodes.push_back(content.nodes.at(n));
            }
        }
    }

    return content;
}

}

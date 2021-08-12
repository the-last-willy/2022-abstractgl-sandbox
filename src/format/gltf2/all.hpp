#pragma once

#include <tiny_gltf.h>

#include "engine/database.hpp"

namespace format::gltf2 {

void fill(eng::Database& database, tinygltf::Model& model) {
    auto buffer_mapping = std::map<int, agl::Buffer>();
    { // Converting buffers.
        for(std::size_t i = 0; i < size(model.buffers); ++i) {
            auto b = agl::create(agl::buffer_tag);
            agl::storage(b, std::span(model.buffers[i].data));
            buffer_mapping[static_cast<int>(i)] = b;
            database.gl_buffers[i] = b;
        }
        
    }
    auto image_mapping = std::map<int, agl::Texture>();
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
            image_mapping[static_cast<int>(i)] = t;
            database.gl_textures.push_back(t);
        }
    }
    auto texture_mapping = std::map<int, agl::Texture>();
    { // Converting textures.
        for(std::size_t i = 0; i < size(model.textures); ++i) {
            auto& texture = model.textures[i];
            texture_mapping[static_cast<int>(i)]
            = image_mapping.at(texture.source);
        }
    }
    auto material_mapping = std::map<int, eng::Material*>();
    { // Converting materials.
        for(std::size_t i = 0; i < size(model.materials); ++i) {
            auto& material = model.materials[i];
            auto eng_material = database.default_material;
            { // 'doubleSided'.
                if(!material.doubleSided) {
                    eng_material.program.capabilities.push_back(agl::Capability::cull_face);
                    auto f = eng_material.on_enter;
                    eng_material.on_enter = [f = eng_material.on_enter]() {
                        f();
                        glCullFace(GL_FRONT);
                    };
                }
            }
            { // 'pbrMetallicRoughness'.
                auto& pbrMetallicRoughness = material.pbrMetallicRoughness;  
                { // 'baseColorTexture'.
                    auto& baseColorTexture = pbrMetallicRoughness.baseColorTexture;
                    if(baseColorTexture.index != -1) {
                        eng_material.textures.push_back(std::make_tuple(
                            std::string("baseColorTexture"),
                            texture_mapping.at(baseColorTexture.index)));
                    } else {
                        eng_material.textures.push_back(std::make_tuple(
                            std::string("baseColorTexture"),
                            database.default_albedo_map));
                    }
                }
            }
            { // 'normalTexture'.
                auto& normalTexture = material.normalTexture;  
                if(normalTexture.index != -1) {
                    eng_material.textures.push_back(std::make_tuple(
                            std::string("normalTexture"),
                            texture_mapping.at(normalTexture.index)));
                } else {
                    eng_material.textures.push_back(std::make_tuple(
                            std::string("normalTexture"),
                            database.default_normal_map));
                }
            }
            database.materials.push_back(std::move(eng_material));
            material_mapping[static_cast<int>(i)] = &database.materials.back();
        }
    }
    { // Converting primitives.
        for(auto& mesh : model.meshes)
        for(auto& primitive : mesh.primitives) {
            auto p = eng::Primitive();
            {
                p.draw_mode = static_cast<agl::DrawMode>(primitive.mode);
            }
            auto gl_vertex_array = p.vertex_array = agl::vertex_array();
            { // Indices.
                auto& accessor = model.accessors[primitive.indices];
                auto& buffer_view = model.bufferViews[accessor.bufferView];
                p.draw_type = static_cast<agl::DrawType>(accessor.componentType);
                p.offset = buffer_view.byteOffset;
                p.primitive_count = agl::Count<GLsizei>(
                    static_cast<GLsizei>(accessor.count));
                agl::element_buffer(
                    gl_vertex_array,
                    buffer_mapping.at(buffer_view.buffer));
            }
            auto& material = *(p.material = material_mapping.at(primitive.material));
            for(int i = 0; i < agl::active_attributes(material.program.program); ++i) {
                auto ai = agl::AttributeIndex(i);
                auto aa = agl::active_attrib(material.program.program, ai);
                auto bi = agl::BindingIndex<GLuint>(i);
                attribute_binding(gl_vertex_array, ai, bi);

                auto it = primitive.attributes.find(aa.name);
                if(it != end(primitive.attributes)) {
                    auto& accessor = model.accessors[it->second];
                    auto& buffer_view = model.bufferViews[accessor.bufferView];
                    auto size = agl::Size<GLint>();
                    switch(accessor.type) {
                    case TINYGLTF_TYPE_SCALAR:
                        size = agl::Size<GLint>(1);
                        break;
                    case TINYGLTF_TYPE_VEC2:
                        size = agl::Size<GLint>(2);
                        break;
                    case TINYGLTF_TYPE_VEC3:
                        size = agl::Size<GLint>(3);
                        break;
                    case TINYGLTF_TYPE_VEC4:
                        size = agl::Size<GLint>(4);
                        break;
                    default:
                        throw std::runtime_error("Unhandled accessor type.");
                        break;
                    }
                    attribute_format(gl_vertex_array, ai,
                        size,
                        accessor.componentType,
                        agl::Normalized(accessor.normalized),
                        agl::Offset<GLuint>(static_cast<GLuint>(accessor.byteOffset)));
                    auto stride = agl::Stride<GLsizei>(static_cast<GLsizei>(buffer_view.byteStride));
                    auto component_byte_size = 0;
                    if(stride.value == 0) {
                        switch(accessor.componentType) {
                        case GL_FLOAT:
                            component_byte_size = 4;
                            break;
                        default:
                            throw std::runtime_error("Unhandled component type.");
                            break;
                        }
                        stride = agl::Stride<GLsizei>(size.value * component_byte_size);
                    }
                    vertex_buffer(gl_vertex_array, bi,
                            buffer_mapping[buffer_view.buffer], 
                            agl::Offset<GLintptr>(buffer_view.byteOffset),
                            stride);
                    enable(gl_vertex_array, ai);
                } else {
                    // throw std::runtime_error("Failed to find attribute.");
                }
            }
            database.primitives.push_back(std::move(p));
        }
    }
}

}

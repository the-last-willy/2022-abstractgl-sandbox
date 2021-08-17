#pragma once

#include <engine/all.hpp>

#include <tiny_gltf.h>

#include <map>

namespace format::gltf2 {

struct Content {
    std::map<int, eng::Accessor> accessors = {};
    std::map<int, agl::Buffer> buffers = {};
    std::map<int, agl::Texture> images = {};
    std::map<int, eng::Material> materials = {};
    std::map<int, eng::Texture> textures = {};
};

inline
auto fill(eng::Database& database, tinygltf::Model& model) {
    auto content = Content();

    { // Converting buffers.
        for(std::size_t i = 0; i < size(model.buffers); ++i) {
            auto b = agl::create(agl::buffer_tag);
            agl::storage(b, std::span(model.buffers[i].data));
            content.buffers[static_cast<int>(i)] = b;
            database.gl_buffers[i] = b;
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
            content.images[static_cast<int>(i)] = t;
            database.gl_textures.push_back(t);
        }
    }

    { // Converting textures.
        for(std::size_t i = 0; i < size(model.textures); ++i) {
            auto& texture = model.textures[i];
            content.textures[static_cast<int>(i)] = { content.images.at(texture.source) };
        }
    }

    { // Converting materials.
        for(std::size_t i = 0; i < size(model.materials); ++i) {
            auto& material = model.materials[i];
            auto eng_material = database.default_material;
            { // 'doubleSided'.
                if(!material.doubleSided) {
                    eng_material.program.capabilities.push_back({
                        agl::Capability::cull_face, []() {
                            glCullFace(GL_FRONT); }});
                }
            }
            { // 'pbrMetallicRoughness'.
                auto& pbrMetallicRoughness = material.pbrMetallicRoughness;  
                { // 'baseColorTexture'.
                    auto& baseColorTexture = pbrMetallicRoughness.baseColorTexture;
                    if(baseColorTexture.index != -1) {
                        eng_material.textures.emplace(
                            "baseColorTexture",
                            content.textures.at(baseColorTexture.index).texture);
                    } else {
                        eng_material.textures.emplace(
                            "baseColorTexture",
                            database.default_albedo_map);
                    }
                }
            }
            { // 'normalTexture'.
                auto& normalTexture = material.normalTexture;  
                if(normalTexture.index != -1) {
                    eng_material.textures.emplace(
                            "normalTexture",
                            content.textures.at(normalTexture.index).texture);
                } else {
                    eng_material.textures.emplace(
                            "normalTexture",
                            database.default_normal_map);
                }
            }
            database.materials.push_back(std::move(eng_material));
            content.materials[static_cast<int>(i)] = database.materials.back();
        }
    }

    { // Converting accessors.
        for(std::size_t i = 0; i < size(model.accessors); ++i) {
            auto& accessor = model.accessors[i];

            auto eng_accessor = eng::Accessor();

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

            eng_accessor.buffer_view_byte_stride
            = agl::Stride<GLsizei>(static_cast<GLsizei>(buffer_view.byteStride));

            if(eng_accessor.buffer_view_byte_stride.value == 0) {
                switch(accessor.componentType) {
                case GL_FLOAT:
                    eng_accessor.component_size = 4;
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

            content.accessors[static_cast<int>(i)] = std::move(eng_accessor);
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
                    content.buffers.at(buffer_view.buffer));
            }
            auto& material = (p.material = content.materials.at(primitive.material));
            for(int i = 0; i < agl::active_attributes(material.program.program); ++i) {
                auto ai = agl::AttributeIndex(i);
                auto aa = agl::active_attrib(material.program.program, ai);
                auto bi = agl::BindingIndex<GLuint>(i);
                attribute_binding(gl_vertex_array, ai, bi);

                auto it = primitive.attributes.find(aa.name);
                if(it != end(primitive.attributes)) {
                    auto& accessor = content.accessors.at(it->second);

                    attribute_format(gl_vertex_array, ai,
                        accessor.component_count,
                        accessor.component_type,
                        accessor.normalized,
                        accessor.byte_offset);
                    
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
                            content.buffers[buffer_view.buffer], 
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

    return content;
}

}

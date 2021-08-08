// Local headers.

#include "engine/all.hpp"
#include "program/all.hpp"
#include "scene/all.hpp"
#include "file.hpp"
#include "root.hpp"

// glfw

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// tinygltf
// https://github.com/syoyo/tinygltf

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

// Standard library.

#include <iostream>

//

eng::Material gltf_normal_mapping_material = {};
eng::Material gltf_no_normal_mapping_material = {};

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
            auto t = agl::texture(agl::TextureTarget::_2d);
            mag_filter(t, GL_LINEAR);
            min_filter(t, GL_LINEAR);
            if(image.component == 3 && image.bits == 8) {
                agl::storage(
                    t, GL_RGB8,
                    agl::Width(image.width), agl::Height(image.height));
                agl::image(
                    t, agl::Width(image.width), agl::Height(image.height),
                    GL_RGB, GL_UNSIGNED_BYTE,
                    std::as_bytes(std::span(image.image)));
            } else if(image.component == 4 && image.bits == 8) {
                agl::storage(
                    t, GL_RGBA8,
                    agl::Width(image.width), agl::Height(image.height));
                agl::image(
                    t, agl::Width(image.width), agl::Height(image.height),
                    GL_RGBA, GL_UNSIGNED_BYTE,
                    std::as_bytes(std::span(image.image)));
                glFlush();
            } else {
                throw std::runtime_error("Invalid texture format.");
            }
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
            auto eng_material = gltf_no_normal_mapping_material;
            { // 'pbrMetallicRoughness'.
                auto& pbrMetallicRoughness = material.pbrMetallicRoughness;  
                { // 'baseColorTexture'.
                    auto& baseColorTexture = pbrMetallicRoughness.baseColorTexture;
                    if(baseColorTexture.index != -1) {
                        eng_material.textures.push_back(std::make_tuple(
                            std::string("baseColorTexture"),
                            texture_mapping.at(baseColorTexture.index)));
                    }
                }
            }
            { // 'normalTexture'.
                auto& normalTexture = material.normalTexture;  
                if(normalTexture.index != -1) {
                    eng_material.textures.push_back(std::make_tuple(
                            std::string("normalTexture"),
                            texture_mapping.at(normalTexture.index)));
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
            if(primitive.attributes.find("TANGENT") != end(primitive.attributes)) {
                material.program = gltf_normal_mapping_material.program;
            }
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
                    throw std::runtime_error("Failed to find attribute.");
                }
            }
            database.primitives.push_back(std::move(p));
        }
    }
    // auto texture_mapping = std::map<int, std::size_t>();
    // { // Converting textures.
    //     for(std::size_t i = 0; i < size(model.buffers); ++i) {
    //         auto b = agl::buffer();
    //         agl::storage(b, std::span(model.buffers[i]));
    //         buffer_mapping[i] = size(database.buffers);
    //         database.buffers[i] = std::move(b);
    //     }
    // }
    // auto material_mapping = std::map<int, std::size_t>();
    // { // Converting materials.
    //     for(std::size_t i = 0; i < size(model.buffers); ++i) {
    //         auto b = agl::buffer();
    //         agl::storage(b, std::span(model.buffers[i]));
    //         buffer_mapping[i] = size(database.buffers);
    //         database.buffers[i] = std::move(b);
    //     }
    // }
    // auto primitive_mapping = std::map<int, std::size_t>();
    // { // Converting primitives.
    //     for(std::size_t i = 0; i < size(model.primitives); ++i) {
    //         auto b = agl::buffer();
    //         agl::storage(b, std::span(model.buffers[i]));
    //         { // 

    //         }
    //         {

    //         }
    //         buffer_mapping[i] = size(database.buffers);
    //         database.buffers[i] = std::move(b);
    //     }
    // }
}

struct GltfProgram : Program {
    eng::Database database = {};

    tlw::PerspectiveProjection proj = {};
    tlw::View view = {};
    
    void init() override {
        { // Loading glTF rendering material.
            {  // No normal mapping.
                load(gltf_no_normal_mapping_material.program, {
                    {
                        agl::ShaderType::vertex,
                        file(tlw::root + "src/shader/gltf/phong.vs")
                    },
                    {
                        agl::ShaderType::fragment,
                        file(tlw::root + "src/shader/gltf/phong.fs")
                    }});
                gltf_no_normal_mapping_material.program.capabilities = {
                    agl::Capability::cull_face,
                    agl::Capability::depth_test};
                gltf_no_normal_mapping_material.on_enter = []() {
                    glDepthFunc(GL_LESS);
                    glCullFace(GL_FRONT);
                };
            }
            { // Normal mapping.
                load(gltf_normal_mapping_material.program, {
                    {
                        agl::ShaderType::vertex,
                        file(tlw::root + "src/shader/gltf/phong_normal_mapping.vs")
                    },
                    {
                        agl::ShaderType::fragment,
                        file(tlw::root + "src/shader/gltf/phong_normal_mapping.fs")
                    }});
                gltf_normal_mapping_material.program.capabilities = {
                    agl::Capability::cull_face,
                    agl::Capability::depth_test};
                gltf_normal_mapping_material.on_enter = []() {
                    glDepthFunc(GL_LESS);
                    glCullFace(GL_FRONT);
                };
            }
        }

        tinygltf::TinyGLTF loader;
        tinygltf::Model model;

        std::string err;
        std::string warn;

        bool ret = loader.LoadASCIIFromFile(
            &model, &err, &warn, 
            "D:/data/sample/gltf2/sponza/Sponza/glTF/Sponza.gltf"
            );

        if (!warn.empty()) {
            std::cerr << "Warning: " << warn << std::endl;
        }

        if (!err.empty()) {
            std::cerr << "Error: " << err << std::endl;
        }

        if (!ret) {
            std::cerr << "Failed to open GLTF file." << std::endl;
            std::exit(EXIT_FAILURE);
        }

        fill(database, model);

        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void update() override {
        {
            agl::Vec2 d = current_cursor_pos - previous_cursor_pos;
            view.yaw += d[0] / 500.f;
            view.pitch += d[1] / 500.f;

            previous_cursor_pos = current_cursor_pos;
        }
        {
            if(glfwGetKey(window.window, GLFW_KEY_S)) {
                auto direction = rotation(view)[2].xyz();
                view.position = view.position - direction;
            }
            if(glfwGetKey(window.window, GLFW_KEY_W)) {
                auto direction = rotation(view)[2].xyz();
                view.position = view.position + direction;
            }
        }
    }

    void render() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto inv_v = transform(view);
        auto v = inverse(inv_v);

        auto mvp = transform(proj) * v * agl::scaling3(0.1f);
        auto normal_transform = transpose(inv_v);

        for(auto& p : database.primitives) {
            bind(p);

            agl::uniform(
                p.material->program.program,
                *agl::uniform_location(p.material->program.program, "mvp"),
                mvp);
            agl::uniform(
                p.material->program.program,
                *agl::uniform_location(p.material->program.program, "normal_transform"),
                normal_transform);

            eng::render(p);

            unbind(p);
        }
    }
};

void throwing_main() {
    auto p = GltfProgram();
    run(p);
}

int main() {
    try {
        throwing_main();
        return 0;
    } catch(const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Unhandled exception." << std::endl;
    }
    return -1;
}

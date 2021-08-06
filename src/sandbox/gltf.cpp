// Local headers.

#include "engine/database.hpp"
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

agl::Program gltf_program = {};

void fill(eng::Database& database, tinygltf::Model& model) {
    // auto buffer_mapping = std::map<int, agl::Buffer>();
    // { // Converting buffers.
    //     for(std::size_t i = 0; i < size(model.buffers); ++i) {
    //         auto b = agl::buffer();
    //         agl::storage(b, std::span(model.buffers[i].data));
    //         buffer_mapping[static_cast<int>(i)] = b;
    //         database.gl_buffers[i] = b;
    //     }
        
    // }
    auto buffer_view_mapping = std::map<int, agl::Buffer>();
    {
        // Exploding attribute buffers.
        for(auto& mesh : model.meshes)
        for(auto& primitive : mesh.primitives)
        for(auto [name, accessor_id] : primitive.attributes) {
            auto& buffer_view_id = model.accessors[accessor_id].bufferView;
            auto& buffer_view = model.bufferViews[buffer_view_id];
            auto& buffer = model.buffers[buffer_view.buffer];
            auto b = agl::buffer();
            auto ss = std::span(buffer.data).subspan(
                buffer_view.byteOffset, buffer_view.byteLength);
            agl::storage(b, ss);
            buffer_view_mapping[buffer_view_id] = b;
        }
        // Exploding index buffers.
        for(auto& mesh : model.meshes)
        for(auto& primitive : mesh.primitives) {
            auto indices = primitive.indices;
            auto& buffer_view = model.bufferViews[model.accessors[indices].bufferView];
            auto& buffer = model.buffers[buffer_view.buffer];
            auto b = agl::buffer();
            auto ss = std::span(buffer.data).subspan(
                buffer_view.byteOffset, buffer_view.byteLength);
            agl::storage(b, ss);
            buffer_view_mapping[indices] = b;
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
                p.draw_type = static_cast<agl::DrawType>(accessor.componentType);
                p.primitive_count = agl::Count<GLsizei>(
                    static_cast<GLsizei>(accessor.count));
                agl::element_buffer(
                    gl_vertex_array,
                    buffer_view_mapping.at(accessor.bufferView));
            }
            for(int i = 0; i < agl::active_attributes(gltf_program); ++i) {
                auto ai = agl::AttributeIndex(i);
                auto aa = agl::active_attrib(gltf_program, ai);
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
                            buffer_view_mapping[accessor.bufferView], 
                            agl::Offset<GLintptr>(0),
                            stride);
                    enable(gl_vertex_array, ai);
                } else {
                    throw std::runtime_error("Failed to find attribute.");
                }
            }
            database.primitives.push_back(std::move(p));
        }
    }
    auto image_mapping = std::map<int, std::size_t>();
    { // Converting images.
        for(std::size_t i = 0; i < size(model.images); ++i) {
            auto& image = model.images[i];
            auto t = agl::texture(agl::TextureTarget::_2d);
            if(image.component == 3 && image.bits == 8) {
                agl::storage(
                    t, GL_RGB8,
                    agl::Width(image.width), agl::Height(image.height));
                agl::image(
                    t, agl::Width(0), agl::Height(0),
                    GL_RGB, GL_UNSIGNED_BYTE,
                    std::as_bytes(std::span(image.image)));
            } else if(image.component == 4 && image.bits == 8) {
                agl::storage(
                    t, GL_RGBA8,
                    agl::Width(image.width), agl::Height(image.height));
                agl::image(
                    t, agl::Width(0), agl::Height(0),
                    GL_RGBA, GL_UNSIGNED_BYTE,
                    std::as_bytes(std::span(image.image)));
            } else {
                throw std::runtime_error("Invalid texture format.");
            }
            image_mapping[static_cast<int>(i)] = i;
            database.gl_textures[i] = std::move(t);
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
            gltf_program = agl::program();
            auto shaders = std::map<agl::ShaderType, std::string>{
                {
                    agl::ShaderType::vertex,
                    file(tlw::root + "src/shader/gltf/normal.vs")
                },
                {
                    agl::ShaderType::fragment,
                    file(tlw::root + "src/shader/gltf/normal.fs")
                },
            };
            for(auto& [type, src] : shaders) {
                auto s = agl::shader(type);
                agl::source(s, src);
                agl::compile(s);
                agl::attach(gltf_program, s);
                agl::delete_(s);
            }
            agl::link(gltf_program);
        }

        tinygltf::TinyGLTF loader;
        tinygltf::Model model;

        std::string err;
        std::string warn;

        bool ret = loader.LoadASCIIFromFile(
            &model, &err, &warn, 
            "D:/data/sample/gltf2/sponza/Sponza/glTF/Sponza.gltf"
            // "D:/data/sample/gltf2/box/Box/glTF/Box.gltf"
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

        glCullFace(GL_FRONT);
        agl::enable(agl::Capability::cull_face);
        glDepthFunc(GL_LESS);
        agl::enable(agl::Capability::depth_test);

        auto inv_v = transform(view);
        auto v = inverse(inv_v);

        auto mvp = transform(proj) * v * agl::scaling3(0.1f);
        auto normal_transform = transpose(inv_v);

        agl::use(gltf_program);

        agl::uniform(
            gltf_program,
            agl::uniform_location(gltf_program, "mvp"),
            mvp);
        agl::uniform(
            gltf_program,
            agl::uniform_location(gltf_program, "normal_transform"),
            normal_transform);

        for(auto& p : database.primitives) {
            agl::bind(p.vertex_array);
            if(agl::element_array_buffer_binding(p.vertex_array)) {
                agl::draw_elements(
                    p.draw_mode,
                    p.primitive_count,
                    p.draw_type);
            }
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

#pragma once

#include "face_vertex_mesh.hpp"

#include <agl/engine/all.hpp>

namespace chaine {

inline
auto face_mesh(const FaceVertexMesh& m) {
    auto index_accessor = eng::Accessor();
    {
        auto& accessor = index_accessor;

        {
            auto buffer_ptr = std::make_shared<eng::Buffer>();
            auto& buffer = *buffer_ptr;

            buffer.opengl = create(agl::buffer_tag);
            storage(
                buffer.opengl,
                size(m.faces) * sizeof(agl::Uvec3), 
                GL_MAP_WRITE_BIT);
            auto mapping = map<agl::Uvec3>(buffer.opengl, GL_WRITE_ONLY);
            for(std::size_t i = 0; i < size(m.faces); ++i) {
                auto& f = m.faces[i];
                mapping[i] = agl::Uvec3{
                    static_cast<GLuint>(f.adjacents[0]),
                    static_cast<GLuint>(f.adjacents[1]),
                    static_cast<GLuint>(f.adjacents[2])};
            }
            unmap(buffer.opengl);

            accessor.buffer = buffer_ptr;
        }

        accessor.buffer_view_byte_stride = agl::Stride<GLsizei>(4);
        accessor.component_count = agl::Size<GLint>(1);
        accessor.component_type = GL_UNSIGNED_INT;
        
    }
    auto position_accessor = eng::Accessor();
    {
        auto& accessor = position_accessor;

        {
            auto buffer_ptr = std::make_shared<eng::Buffer>();
            auto& buffer = *buffer_ptr;
            
            buffer.opengl = create(agl::buffer_tag);
            storage(
                buffer.opengl,
                size(m.vertices) * sizeof(agl::Vec3), 
                GL_MAP_WRITE_BIT);
            auto mapping = map<agl::Vec3>(buffer.opengl, GL_WRITE_ONLY);
            for(std::size_t i = 0; i < size(m.vertices); ++i) {
                auto& v = m.vertices[i];
                mapping[i] = v.point;
            }
            unmap(buffer.opengl);

            accessor.buffer = buffer_ptr;
        }

        accessor.buffer_view_byte_stride = agl::Stride<GLsizei>(12);
        accessor.component_count = agl::Size<GLint>(3);
        accessor.component_type = GL_FLOAT;
    }
    
    auto primitive_ptr = std::make_shared<eng::Primitive>();
    {
        auto& primitive = *primitive_ptr;
        primitive.attributes["POSITION"] = std::move(position_accessor);
        primitive.draw_type = agl::DrawType::unsigned_int;
        primitive.indices = std::move(index_accessor);
        primitive.primitive_count = agl::Count<GLsizei>(3 * GLsizei(size(m.faces)));
        primitive.vertex_array = agl::vertex_array();
    }

    return std::make_shared<eng::Mesh>(eng::Mesh({std::move(primitive_ptr)}));
}

}
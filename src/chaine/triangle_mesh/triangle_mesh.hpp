#pragma once

#include <agl/all.hpp>
#include <agl/engine/all.hpp>

#include <memory>
#include <vector>

namespace chaine {

struct TriangleMesh {
    std::vector<agl::Uvec3> triangle_indices = {};
    std::vector<agl::Vec3> vertex_positions = {};
};

inline
auto face_mesh(const TriangleMesh& m) {
    auto index_accessor = eng::Accessor();
    {
        auto& accessor = index_accessor;

        {
            auto buffer_ptr = std::make_shared<eng::Buffer>();
            auto& buffer = *buffer_ptr;

            buffer.opengl = create(agl::buffer_tag);
            storage(
                buffer.opengl,
                size(m.triangle_indices) * sizeof(agl::Uvec3), 
                GL_MAP_WRITE_BIT);
            auto mapping = map<agl::Uvec3>(buffer.opengl, GL_WRITE_ONLY);
            for(std::size_t i = 0; i < size(m.triangle_indices); ++i) {
                mapping[i] = m.triangle_indices[i];
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
                size(m.vertex_positions) * sizeof(agl::Vec3), 
                GL_MAP_WRITE_BIT);
            auto mapping = map<agl::Vec3>(buffer.opengl, GL_WRITE_ONLY);
            for(std::size_t i = 0; i < size(m.vertex_positions); ++i) {
                mapping[i] = m.vertex_positions[i];
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
        primitive.primitive_count = agl::Count<GLsizei>(GLsizei(3 * size(m.triangle_indices)));
    }

    return std::make_shared<eng::Mesh>(eng::Mesh({std::move(primitive_ptr)}));
}

inline
auto edge_mesh(const TriangleMesh& m) {
    auto index_accessor = eng::Accessor();
    {
        auto& accessor = index_accessor;

        {
            auto buffer_ptr = std::make_shared<eng::Buffer>();
            auto& buffer = *buffer_ptr;

            buffer.opengl = create(agl::buffer_tag);
            storage(
                buffer.opengl,
                6 * size(m.vertex_positions) * sizeof(agl::Uvec3), 
                GL_MAP_WRITE_BIT);
            auto mapping = map<GLuint>(buffer.opengl, GL_WRITE_ONLY);
            for(std::size_t i = 0; i < size(m.triangle_indices); ++i) {
                mapping[6 * i + 0] = m.triangle_indices[i][0];
                mapping[6 * i + 1] = m.triangle_indices[i][1];
                mapping[6 * i + 2] = m.triangle_indices[i][1];
                mapping[6 * i + 3] = m.triangle_indices[i][2];
                mapping[6 * i + 4] = m.triangle_indices[i][2];
                mapping[6 * i + 5] = m.triangle_indices[i][0];
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
                size(m.vertex_positions) * sizeof(agl::Vec3), 
                GL_MAP_WRITE_BIT);
            auto mapping = map<agl::Vec3>(buffer.opengl, GL_WRITE_ONLY);
            for(std::size_t i = 0; i < size(m.vertex_positions); ++i) {
                mapping[i] = m.vertex_positions[i];
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
        primitive.draw_mode = agl::DrawMode::lines;
        primitive.draw_type = agl::DrawType::unsigned_int;
        primitive.indices = std::move(index_accessor);
        primitive.primitive_count = agl::Count<GLsizei>(6 * GLsizei(size(m.triangle_indices)));
        primitive.vertex_array = agl::vertex_array();
    }

    return std::make_shared<eng::Mesh>(eng::Mesh({std::move(primitive_ptr)}));
}


inline
auto vertex_mesh(const TriangleMesh& m) {
    auto index_accessor = eng::Accessor();
    {
        auto& accessor = index_accessor;

        {
            auto buffer_ptr = std::make_shared<eng::Buffer>();
            auto& buffer = *buffer_ptr;

            buffer.opengl = create(agl::buffer_tag);
            storage(
                buffer.opengl,
                size(m.triangle_indices) * sizeof(agl::Uvec3), 
                GL_MAP_WRITE_BIT);
            auto mapping = map<agl::Uvec3>(buffer.opengl, GL_WRITE_ONLY);
            for(std::size_t i = 0; i < size(m.triangle_indices); ++i) {
                mapping[i] = m.triangle_indices[i];
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
                size(m.vertex_positions) * sizeof(agl::Vec3), 
                GL_MAP_WRITE_BIT);
            auto mapping = map<agl::Vec3>(buffer.opengl, GL_WRITE_ONLY);
            for(std::size_t i = 0; i < size(m.vertex_positions); ++i) {
                mapping[i] = m.vertex_positions[i];
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
        primitive.draw_mode = agl::DrawMode::points;
        primitive.draw_type = agl::DrawType::unsigned_int;
        primitive.indices = std::move(index_accessor);
        primitive.primitive_count = agl::Count<GLsizei>(3 * GLsizei(size(m.triangle_indices)));
    }

    return std::make_shared<eng::Mesh>(eng::Mesh({std::move(primitive_ptr)}));
}

}

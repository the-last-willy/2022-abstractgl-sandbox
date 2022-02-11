#pragma once

#include "../solid_uv_sphere.hpp"
#include "glsl/wireframe_renderer/wireframe_renderer.hpp"

// inline
// gl::VertexArrayObj vertex_array(const Solid_UV_Sphere& suvs, const WireframeRenderer& wr) {
//     auto va = gl::VertexArrayObj();
//     { // Normals, positions.
//         auto bindingindex = GLuint(0);
//         gl::VertexArrayAttribFormat(va,
//             wr.color,
//             3, GL_FLOAT,
//             GL_FALSE, 0);
//         gl::VertexArrayVertexBuffer(va,
//             bindingindex,
//             suvs.normals_positions,
//             0, sizeof(glm::vec3));
//         gl::VertexArrayAttribBinding(va,
//             wr.color,
//             bindingindex);
//         gl::EnableVertexArrayAttrib(va,
//             wr.color);
//     }
//     return va;
// }

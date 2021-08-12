#version 460 core

uniform mat4 mv;
uniform mat4 mvp;
uniform mat4 normal_transform;

layout(location = 0) in vec3 NORMAL;
layout(location = 1) in vec3 POSITION;
layout(location = 2) in vec4 TANGENT;
layout(location = 3) in vec2 TEXCOORD_0;

out vec3 vertex_normal;
out vec3 vertex_position;
out vec4 vertex_tangent;
out vec2 vertex_texcoord;

void main() {
    vertex_normal = normalize((normal_transform * vec4(NORMAL, 0.)).xyz);
    vertex_position = (mv * vec4(POSITION, 1.)).xyz;
    vertex_tangent = vec4(
        normalize((normal_transform * vec4(TANGENT.xyz, 0.)).xyz),
        TANGENT.w);
    vertex_texcoord = TEXCOORD_0;

    gl_Position = mvp * vec4(POSITION, 1.);
}

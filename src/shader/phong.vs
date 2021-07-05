#version 460

uniform mat4 mv;
uniform mat4 mvp;
uniform mat4 normal_matrix;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec3 position;
layout(location = 2) in vec2 uv;

out vec3 vertex_normal;
out vec3 vertex_position;

void main() {
    vertex_normal = (normal_matrix * vec4(normal, 0.f)).xyz;
    vertex_position = (mv * vec4(position, 1.f)).xyz;

    gl_Position = mvp * vec4(position.xy, position.z, 1.f);
}

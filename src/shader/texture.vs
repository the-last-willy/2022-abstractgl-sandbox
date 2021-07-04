#version 460

uniform mat4 m;
uniform mat4 mvp;

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;
layout(location = 1) in vec2 uv;

out vec3 vertex_normal;
out vec2 vertex_uv;

void main() {
    vertex_normal = (m * vec4(normal, 1.f)).xyz;
    vertex_uv = uv;

    gl_Position = mvp * vec4(position, 1.f);
}

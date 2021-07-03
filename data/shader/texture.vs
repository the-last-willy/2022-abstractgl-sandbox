#version 460

uniform mat4 mvp;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 vertex_uv;

void main() {
    vertex_uv = uv;

    gl_Position = mvp * vec4(position, 1.f);
}

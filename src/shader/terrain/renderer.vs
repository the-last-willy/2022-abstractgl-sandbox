#version 460 core

uniform mat4 mv;
uniform mat4 mvp;

layout(location = 0) in vec2 coords;
layout(location = 1) in float height;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

out vec3 vertex_normal;
out vec2 vertex_uv;

void main() {
    vec4 position = vec4(coords.x, height, coords.y, 1.f);

    vertex_normal = normal;
    vertex_uv = uv;

    gl_Position = mvp * position;
    gl_Position.z = -1.f;
}

#version 460 core

uniform mat4 mvp;
uniform mat4 normal_transform;

layout(location = 0) in vec3 NORMAL;
layout(location = 1) in vec3 POSITION;

out vec3 vertex_normal;

void main() {
    vertex_normal = normalize(normal_transform * vec4(NORMAL, 0.)).xyz;

    gl_Position = mvp * vec4(POSITION, 1.);
}

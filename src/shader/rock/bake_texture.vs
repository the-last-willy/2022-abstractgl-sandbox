#version 460 core

layout(location = 0) in vec3 position3;
layout(location = 1) in vec2 texcoords2;

out vec3 vertex_position3;

void main() {
    vertex_position3 = position3;

    gl_Position = vec4(texcoords2 * 2. - 1., 0., 1.);
}

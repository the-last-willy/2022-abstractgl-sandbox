#version 460 core

in vec3 vertex_normal3;

out vec4 color;

void main() {
    // color = vec4(vec3(1.f), 1.f);
    color = vec4(vertex_normal3 / 2.f + 0.5f, 1.f);
}

#version 450 core

in vec3 vertex_color;

out vec3 fragment_color;

void main() {
    fragment_color = vertex_color;
}

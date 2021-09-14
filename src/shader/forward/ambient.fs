#version 450

layout(location = 0) in vec3 vertex_color;

out vec3 fragment_color;

void main() {
    fragment_color = vertex_color;
}

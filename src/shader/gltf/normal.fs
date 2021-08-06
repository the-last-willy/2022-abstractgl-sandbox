#version 460 core

in vec3 vertex_normal;

out vec3 fragment_color;

void main() {
    vec3 normal = normalize(vertex_normal);

    fragment_color = normal / 2. + .5;
}

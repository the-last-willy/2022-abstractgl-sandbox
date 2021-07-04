#version 460

in vec3 primitive_normal;

out vec4 fragment_color;

void main() {
    fragment_color = vec4(primitive_normal / 2.f + 0.5f, 1.f);
}

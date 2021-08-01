#version 460 core

uniform vec3 color_rgb = vec3(1.f, 1.f, 1.f);

out vec3 fragment_color;

void main() {
    fragment_color = color_rgb;
}

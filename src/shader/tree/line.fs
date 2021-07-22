#version 460 core

uniform vec3 color_rgb;

out vec3 fragment_color;

void main() {
    fragment_color = color_rgb;
}

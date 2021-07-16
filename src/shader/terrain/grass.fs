#version 460 core

uniform vec3 color = vec3(0.f, 1.f, 0.f);

out vec4 fragment_color;

void main() {
    fragment_color = vec4(color, 1.f);
}

#version 450

out vec3 fragment_rgb;

void main() {
    fragment_rgb = vec3(gl_FrontFacing ? 1. : 0.);
}

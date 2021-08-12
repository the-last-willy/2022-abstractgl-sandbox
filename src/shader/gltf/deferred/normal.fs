#version 460 core

uniform sampler2D normal_texture;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {
    fragment_rgb = texture(normal_texture, vertex_texcoords).rgb / 2. + .5;;
}

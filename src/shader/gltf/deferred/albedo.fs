#version 460 core

uniform sampler2D albedo_texture;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {
    fragment_rgb = texture(albedo_texture, vertex_texcoords).rgb;
}

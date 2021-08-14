#version 460 core

uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;

uniform vec3 light_direction;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {
    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    vec3 position = texture(position_texture, vertex_texcoords).rgb;

    // float factor = .5 * (1. - min(length(position) / 100., 1.));
    float factor = .5;
    fragment_rgb = factor * albedo;
}

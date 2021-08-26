#version 460 core

uniform sampler2D albedo_texture;
uniform sampler2D emissive_texture;
uniform sampler2D normal_texture;
uniform sampler2D occlusion_texture;
uniform sampler2D position_texture;

uniform float occlusionStrength = 1.;

uniform vec3 light_direction;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {
    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    vec3 emissive = texture(emissive_texture, vertex_texcoords).rgb;
    float occlusion = texture(occlusion_texture, vertex_texcoords).r;
    vec3 position = texture(position_texture, vertex_texcoords).rgb;

    float factor = .1;
    vec3 ambient = factor * albedo;
    fragment_rgb = emissive + mix(ambient, occlusion * ambient, occlusionStrength);
}

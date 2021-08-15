#version 460 core

uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;
uniform samplerCubeArray shadow_map;

uniform vec3 light_position;
uniform vec3 view_position;

uniform float far;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {
    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    vec3 normal = texture(normal_texture, vertex_texcoords).xyz;
    // View space.
    vec3 position = texture(position_texture, vertex_texcoords).xyz;

    float position_depth = length(position - light_position) / 100.f;

    fragment_rgb = vec3(position_depth);

}

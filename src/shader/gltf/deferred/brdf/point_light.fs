#version 460 core

const float pi = 3.14159265359;

#include "brdf/distribution_ggx.glsl"

uniform sampler2D albedo_texture;
uniform sampler2D metallic_roughness_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;

uniform vec3 light_position;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

float geometry_schlick_ggx_direct(vec3 n, vec3 v, float k) {
    float alpha = k * k;
    float k_direct = (alpha + 1.) * (alpha + 1.) / 8.;
    float n_dot_v = dot(n, v);
    return n_dot_v / (n_dot_v * (1. - k_direct) + k_direct);
}

float geometry_schlick_ggx_direct(vec3 n, vec3 v, vec3 l, float k) {
    return geometry_schlick_ggx_direct(n, v, k)
    * geometry_schlick_ggx_direct(n, l, k);
}

void main() {
    vec3 metallic_roughness = texture(metallic_roughness_texture, vertex_texcoords).rgb;

    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    float metallic = metallic_roughness.b;
    vec3 normal = texture(normal_texture, vertex_texcoords).xyz;
    vec3 position = texture(position_texture, vertex_texcoords).xyz;
    float roughness = metallic_roughness.g;

    vec3 light_direction = -normalize(position - light_position);
    vec3 view_direction = -normalize(position);
    vec3 halfway_direction = normalize(light_direction + view_direction);

    fragment_rgb = vec3(distribution_ggx(normal, halfway_direction, roughness)
    * geometry_schlick_ggx_direct(normal, view_direction, halfway_direction, roughness));

    // if(gl_FragCoord.x < 640) {
    //     fragment_rgb = vec3(geometry_schlick_ggx_direct(normal, view_direction, roughness));
    // } else {
    //     fragment_rgb = vec3(geometry_schlick_ggx_direct(normal, light_direction, roughness));
    // }

    

    // if(gl_FragCoord.x < 640) {
    //     float fresnel = pow(1. - max(dot(view_direction, normal), 0.), 5.);

    //     fragment_rgb = vec3(fresnel);
    // } else {
    //     float fresnel = pow(1. - max(dot(halfway_direction, normal), 0.), 5.);

    //     fragment_rgb = vec3(fresnel);
    // }
}

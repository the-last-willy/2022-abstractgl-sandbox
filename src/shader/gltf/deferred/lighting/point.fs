#version 460 core

const float pi = 3.14159265359;

#include "brdf/diffuse_lambertian.glsl"
#include "brdf/fresnel_schlick.glsl"
#include "brdf/geometric_schlick.glsl"
#include "brdf/ndf_ggx.glsl"

uniform sampler2D albedo_texture;
uniform sampler2D metallic_roughness_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;
uniform samplerCubeArray shadow_map;

uniform float light_distance = 1000.;
uniform vec3 light_intensity = vec3(1., 1., 1.);
// View space.
uniform vec3 light_position;
// View space -> light space.
uniform mat4 light_transform;

uniform float far;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {   
    vec3 metallic_roughness = texture(metallic_roughness_texture, vertex_texcoords).rgb;

    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    float metalness = metallic_roughness.b;
    vec3 normal = texture(normal_texture, vertex_texcoords).xyz;
    // View space.
    vec3 position = texture(position_texture, vertex_texcoords).xyz;
    float roughness = metallic_roughness.g;

    // (View space).
    vec3 light_direction_vs = normalize(position - light_position);
    // Light space.
    vec3 light_direction_ls = normalize((light_transform * vec4(light_direction_vs, 0.)).xyz);
    light_direction_ls.y *= -1;

    float light_depth = light_distance * texture(shadow_map, vec4(light_direction_ls, 0.)).x;
    float fragment_depth = length(position - light_position);

    float shadow_bias = .0005 * light_distance;;
    bool shadow = light_depth + shadow_bias < fragment_depth;

    // float lambertian = max(dot(normal, -light_direction_vs), 0.);
    // vec3 diffuse = lambertian * albedo;

    // float shininess = 64.;
    // float specular = pow(max(dot(view_direction, -normalize(reflect(light_direction_vs, normal))), 0.), shininess);

    vec3 light_direction = -normalize(position - light_position);
    vec3 view_direction = -normalize(position);
    vec3 halfway_direction = normalize(light_direction + view_direction);

    float h_dot_n = max(dot(halfway_direction, normal), 0.);
    float h_dot_v = max(dot(halfway_direction, view_direction), 0.);
    float l_dot_n = max(dot(light_direction, normal), 0.);
    float n_dot_v = max(dot(normal, view_direction), 0.);

    float alpha = roughness * roughness;
    vec3 F0 = mix(vec3(0.04), albedo, metalness);

    float ndf = ndf_ggx(h_dot_n, alpha);
    float geometric = geometric_schlick(l_dot_n, n_dot_v, alpha);
    vec3 fresnel = fresnel_schlick(h_dot_v, F0);

    vec3 diffuse_brdf = diffuse_lambertian(albedo);
    vec3 specular_brdf = (ndf * geometric * fresnel) / max(4. * l_dot_n * n_dot_v, .01);

    vec3 kd = (1. - fresnel) * (1. - metalness);

    vec3 brdf = (kd * diffuse_brdf + specular_brdf) * light_intensity * dot(normal, light_direction);

    fragment_rgb = (shadow ? vec3(0.) : brdf);
}

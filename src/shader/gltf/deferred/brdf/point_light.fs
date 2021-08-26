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

uniform vec3 light_intensity = vec3(1., 0., 0.);
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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = pi * denom * denom;
	
    return num / denom;
}

void main() {
    vec3 metallic_roughness = texture(metallic_roughness_texture, vertex_texcoords).rgb;

    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    float metalness = metallic_roughness.b;
    vec3 normal = texture(normal_texture, vertex_texcoords).xyz;
    vec3 position = texture(position_texture, vertex_texcoords).xyz;
    float roughness = metallic_roughness.g;

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
    vec3 specular_brdf = (ndf * geometric * fresnel) / max(4. * l_dot_n * n_dot_v, .001);

    vec3 kd = (1. - fresnel) * (1. - metalness);

    fragment_rgb = (kd * diffuse_brdf + specular_brdf) * light_intensity * dot(normal, light_direction);
}

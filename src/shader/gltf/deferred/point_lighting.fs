#version 460 core

uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;

uniform vec3 light_direction;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {
    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    vec3 normal = texture(normal_texture, vertex_texcoords).xyz;
    vec3 position = texture(position_texture, vertex_texcoords).xyz;

    float lambertian = max(dot(normal, -light_direction), 0.);
    vec3 diffuse = (lambertian * .7 + .3) * albedo;

    float shininess = 64.;
    float specular = pow(max(dot(normal, -light_direction), 0.), shininess);

    fragment_rgb = diffuse + specular;
}

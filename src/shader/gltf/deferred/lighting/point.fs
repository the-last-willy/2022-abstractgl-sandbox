#version 460 core

uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;
uniform samplerCubeArray shadow_map;

uniform float light_distance = 1000.;
// View space.
uniform vec3 light_position;
// View space -> light space.
uniform mat4 light_transform;

uniform vec3 view_position;

uniform float far;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {
    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    vec3 normal = texture(normal_texture, vertex_texcoords).xyz;
    // View space.
    vec3 position = texture(position_texture, vertex_texcoords).xyz;

    // View space.
    vec3 view_direction = normalize(position);

    // (View space).
    vec3 light_direction_vs = normalize(position - light_position);
    // Light space.
    vec3 light_direction_ls = normalize((light_transform * vec4(light_direction_vs, 0.)).xyz);
    light_direction_ls.y *= -1;

    float light_depth = light_distance * texture(shadow_map, vec4(light_direction_ls, 0.)).x;
    float fragment_depth = length(position - light_position);

    float shadow_bias = .0005 * light_distance;;
    bool shadow = light_depth + shadow_bias < fragment_depth;

    float lambertian = max(dot(normal, -light_direction_vs), 0.);
    vec3 diffuse = lambertian * albedo;

    float shininess = 64.;
    float specular = pow(max(dot(view_direction, -normalize(reflect(light_direction_vs, normal))), 0.), shininess);

    fragment_rgb = (shadow ? vec3(0.) : 1. * diffuse) + .5 * specular;
}

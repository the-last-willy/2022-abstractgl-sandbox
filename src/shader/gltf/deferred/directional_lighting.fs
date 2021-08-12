#version 460 core

uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;
uniform sampler2D shadow_map;

uniform mat4 light_space_transform;

uniform vec3 light_direction;
uniform vec3 view_position;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {
    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    vec3 normal = texture(normal_texture, vertex_texcoords).xyz;
    vec3 position = texture(position_texture, vertex_texcoords).xyz;

    vec3 light_position = (light_space_transform * vec4(position, 1.f)).xyz;

    vec3 view_direction = normalize(position);

    float lambertian = max(dot(normal, -light_direction), 0.);
    vec3 diffuse = lambertian * albedo;

    float shininess = 64.;
    float specular = pow(max(dot(view_direction, -normalize(reflect(light_direction, normal))), 0.), shininess);

    fragment_rgb = .7 * diffuse + .5 * specular;
    // fragment_rgb = vec3(texture(shadow_map, vertex_texcoords)[0] / 2. + .5);
}

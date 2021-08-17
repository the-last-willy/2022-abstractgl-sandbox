#version 460 core

uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;
uniform sampler2D shadow_map;

// View space -> Light clip space.
uniform mat4 light_space_transform;

uniform vec3 light_direction;

in vec2 vertex_texcoords;

out vec3 fragment_rgb;



uniform float near_plane;
uniform float far_plane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}



void main() {
    vec3 albedo = texture(albedo_texture, vertex_texcoords).rgb;
    vec3 normal = texture(normal_texture, vertex_texcoords).xyz;
    // View space.
    vec3 position = texture(position_texture, vertex_texcoords).xyz;

    // Light clip space.
    vec4 light_position4 = light_space_transform * vec4(position, 1.f);
    vec3 light_position = light_position4.xyz / light_position4.w;
    // Light NDC space.
    light_position = light_position * .5 + .5;

    vec3 view_direction = normalize(position);

    float lambertian = max(dot(normal, -light_direction), 0.);
    vec3 diffuse = lambertian * albedo;

    float shininess = 64.;
    float specular = pow(max(dot(view_direction, -normalize(reflect(light_direction, normal))), 0.), shininess);

    // Light NDC space.
    float shadow_depth = texture(shadow_map, light_position.xy)[0];
    float shadow_bias = .003 / light_position4.w;
    bool shadow = shadow_depth + shadow_bias < light_position.z;

    fragment_rgb = shadow ? vec3(0.) : .7 * diffuse + .5 * specular;
}

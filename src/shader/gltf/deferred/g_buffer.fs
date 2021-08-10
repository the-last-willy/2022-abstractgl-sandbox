#version 460 core

uniform sampler2D baseColorTexture;
// uniform sampler2D normalTexture;

in vec3 vertex_normal;
in vec3 vertex_position;
in vec3 vertex_tangent;
in vec2 vertex_texcoord;

layout(location = 0) out vec4 fragment_albedo;
layout(location = 1) out vec3 fragment_normal;
layout(location = 2) out vec3 fragment_position;

void main() {
    vec3 normal = normalize(vertex_normal);
    vec3 tangent = normalize(vertex_tangent);

    fragment_albedo = texture(baseColorTexture, vertex_texcoord);
    fragment_normal = normal / 2. + .5;
    fragment_position = vertex_position;

    // vec3 bitangent = cross(normal, tangent);

    // mat3 tbn = mat3(tangent, bitangent, normal);
    // vec3 normal_mapping = texture(normalTexture, vertex_texcoord).rgb * 2. - 1.;
    // // Tangent space to view space.
    // vec3 mapped_normal = normal_mapping * tbn;

    // float lambertian = max(dot(mapped_normal, vec3(0., 0., -1.)), 0.);
    // vec4 diffuse = (lambertian / 2. + 0.5) * texture(baseColorTexture, vertex_texcoord).rgba;

    // float shininess = 64.;
    // float specular = pow(max(dot(mapped_normal, vec3(0., 0., -1)), 0.), shininess);

    // fragment_color = diffuse;
}

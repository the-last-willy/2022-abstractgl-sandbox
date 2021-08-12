#version 460 core

uniform sampler2D baseColorTexture;
uniform sampler2D normalTexture;

in vec3 vertex_normal;
in vec3 vertex_position;
in vec4 vertex_tangent;
in vec2 vertex_texcoord;

layout(location = 0) out vec4 fragment_albedo;
layout(location = 1) out vec3 fragment_normal;
layout(location = 2) out vec3 fragment_position;

void main() {
    vec3 normal = normalize(vertex_normal);
    vec3 tangent = normalize(vertex_tangent.xyz);

    vec3 bitangent = vertex_tangent.w * cross(normal, tangent);

    mat3 tbn = mat3(tangent, bitangent, normal);
    vec3 normal_mapping = texture(normalTexture, vertex_texcoord).rgb * 2. - 1.;
    // Tangent space to view space.
    normal = tbn * normal_mapping;

    {
        vec4 albedo = texture(baseColorTexture, vertex_texcoord);
        if(albedo.a < .6) {
            discard;
        } else {
            fragment_albedo = albedo;
        }
    }
    
    fragment_normal = normal;
    fragment_position = vertex_position;
}

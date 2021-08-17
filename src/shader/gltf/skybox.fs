#version 460 core

uniform samplerCubeArray cube_map;

in vec3 vertex_direction;
in vec2 vertex_texcoords;

out vec3 fragment_rgb;

void main() {
    vec3 direction = normalize(vertex_direction);
    fragment_rgb = vec3(texture(cube_map, vec4(normalize(vec3(vertex_texcoords, 1.)), 0.)).r * 10.f);
    // fragment_rgb = vec3(
    //     float(direction.x > 0),
    //     float(direction.y > 0),
    //     float(direction.z > 0));
}

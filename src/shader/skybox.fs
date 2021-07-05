#version 460

uniform samplerCube cube_map;
uniform mat4 view_projection;

in vec4 gl_FragCoord;

out vec4 fragment_color;

void main() {
    vec2 coords = gl_FragCoord.xy / vec2(1280, 720);
    // [0, 1] -> [-1, 1]
    coords = coords * 2.f - 1.f;
    coords.y /= 16/9.f;
    vec3 direction = normalize((view_projection * vec4(coords.xy, 1.f, 0.f)).xyz);
    fragment_color = textureCube(cube_map, direction);
}

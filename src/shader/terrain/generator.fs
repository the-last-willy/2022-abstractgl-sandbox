#version 460 core

out vec4 fragment_height;

void main() {
    float height = cos(length(gl_FragCoord.xy) / 10.f);
    fragment_height = vec4(vec3(height), 1.f);
}

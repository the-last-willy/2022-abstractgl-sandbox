#version 460 core

in float vertex_ndc_z;

out vec3 fragment_color;

void main() {
    fragment_color = vec3(pow(gl_FragCoord.z, 128.));
}

#version 460 core

uniform sampler2D hdr_map;

in vec2 vertex_texcoords;

out vec3 fragment_ldr;

vec3 Tonemap_ACES(vec3 x) {
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}


void main() {
    vec3 hdr = texture(hdr_map, vertex_texcoords).rgb;
    // if(gl_FragCoord.x < 640) {
    //     fragment_ldr = hdr;
    // } else {
    //     float w = 1.2;
    //     hdr = Tonemap_ACES(hdr);
    //     // float gamma = 2.;
    //     // hdr = pow(hdr, vec3(gamma));
    //     fragment_ldr = hdr;
    // }

    // vec3 v = hdr / (hdr + vec3(1.));
    fragment_ldr = pow(hdr, vec3(1. / 2.2)); 

    // fragment_ldr = hdr;
}

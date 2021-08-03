#version 460 core

in vec3 vertex_position3;

out vec3 fragment_color;

vec3 hash(vec3 x) {
    x = vec3(
        dot(x, vec3(127.1, 311.7, 74.7)),
        dot(x, vec3(269.5, 183.3, 246.1)),
        dot(x, vec3(113.5, 271.9, 124.6)));
	return fract(sin(x) * 43758.5453123);
}

// [0, 1] ^ 3
vec3 worley_noise(vec3 v) {
    vec3 d = vec3(1., 1., 1.);
    vec3 i = floor(v);
    for(float j = -1.; j <= 1.; j += 1.)
    for(float k = -1.; k <= 1.; k += 1.)
    for(float l = -1.; l <= 1.; l += 1.) {
        vec3 seed = i + vec3(j, k, l) + hash(i + vec3(j, k, l));
        vec3 diff = seed - v;
        if(dot(diff, diff) < dot(d, d)) {
            d = diff;
        }
    }
    return d;
}

vec3 worley_noise2(vec3 v) {
    vec3 d1 = vec3(1., 1., 1.);
    vec3 d2 = vec3(1., 1., 1.);
    vec3 i = floor(v);
    for(float j = -1.; j <= 1.; j += 1.)
    for(float k = -1.; k <= 1.; k += 1.)
    for(float l = -1.; l <= 1.; l += 1.) {
        vec3 seed = i + vec3(j, k, l) + hash(i + vec3(j, k, l));
        vec3 diff = seed - v;
        if(dot(diff, diff) < dot(d2, d2)) {
            d2 = diff;
            if(dot(d2, d2) < dot(d1, d1)) {
                vec3 tmp = d1;
                d1 = d2;
                d2 = tmp;
            }
        }
    }
    return d2;
}

float worley_noise_edge(vec3 v) {
    vec3 d1 = vec3(1., 1., 1.);
    vec3 d2 = vec3(1., 1., 1.);
    vec3 i = floor(v);
    for(float j = -1.; j <= 1.; j += 1.)
    for(float k = -1.; k <= 1.; k += 1.)
    for(float l = -1.; l <= 1.; l += 1.) {
        vec3 seed = i + vec3(j, k, l) + hash(i + vec3(j, k, l));
        vec3 diff = seed - v;
        if(dot(diff, diff) < dot(d2, d2)) {
            d2 = diff;
            if(dot(d2, d2) < dot(d1, d1)) {
                vec3 tmp = d1;
                d1 = d2;
                d2 = tmp;
            }
        }
    }
    vec3 s1 = v + d1;
    vec3 s2 = v + d2;
    vec3 s1s2 = normalize(s2 - s1);
    vec3 m = (s1 + s2) / 2.;
    vec3 mv = (v - m);
    float d = abs(dot(mv, s1s2));
    return d;
}


void main() {
    // length(vertex_position3): [1, 2]
    float range = length(vertex_position3) - 1.;
    
    // float brightness = range;

    fragment_color = vec3(length(worley_noise_edge(3.f - vertex_position3)) / sqrt(3.) / 2. + 0.5);
}

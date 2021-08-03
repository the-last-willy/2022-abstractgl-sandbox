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
    // Minimum distance to seed vector.
    vec3 md = vec3(1., 1., 1.);
    // Minimum squared radius to seed.
    float mr = 3.f;
    vec3 i = floor(v);
    for(float j = -1.; j <= 1.; j += 1.)
    for(float k = -1.; k <= 1.; k += 1.)
    for(float l = -1.; l <= 1.; l += 1.) {
        vec3 seed = i + vec3(j, k, l) + hash(i + vec3(j, k, l));
        vec3 d = seed - v;
        float r = dot(d, d);
        if(r < mr) {
            md = d;
            mr = r;
        }
    }
    // Minimum distance seed.
    vec3 s1 = v + md;
    // Minimum cell coordinates.
    vec3 mc = floor(s1);
    // Minimum edge distance.
    float me = 3.;
    for(float j = -2.; j <= 2.; j += 1.)
    for(float k = -2.; k <= 2.; k += 1.)
    for(float l = -2.; l <= 2.; l += 1.) {
        if(j == 0. && k == 0. && l == 0.) continue;
        vec3 s2 = mc + vec3(j, k, l) + hash(mc + vec3(j, k, l));
        vec3 d = s1 - v;
        float e = (dot(v - (s1 + s2) / 2., normalize(s1 - s2)));
        if(e < me) {
            me = e;
        }
    }
    return me;
}

void main() {
    // length(vertex_position3): [1, 2]
    float range = length(vertex_position3) - 1.;
    
    float brightness = range;

    float cracks = min(
        smoothstep(0.01, 0.012, worley_noise_edge(vertex_position3)),
        smoothstep(0.001, 0.012, worley_noise_edge(4. * vertex_position3)));

    fragment_color = brightness * vec3(smoothstep(0.01, 0.03, worley_noise_edge(4. * vertex_position3)));
}

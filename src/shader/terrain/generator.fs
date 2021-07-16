#version 460 core

layout(location = 0) out vec4 fragment_color;
layout(location = 1) out float fragment_height;
layout(location = 2) out vec3 fragment_normal;
layout(location = 3) out float fragment_vegetation;

float pi = 3.141593;

vec2 hash2( in vec2 f ) { 
    float n = f.x+131.1*f.y;
    vec2 h =  fract(sin(vec2(n,n+113.0))*43758.5453123); 
    // Domain: [0, 1]^2 -> [-1, 1]^2.
    h = 2.f * h - 1.f;
    return normalize(h);
}

float gradient_noise2(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    // Cubic interpolation.
    // vec2 t = f * f * (3.f -2.f * f);

    // Quintic interpolation.
    vec2 t = f * f * f * (10 + f * (-15 + 6 * f));
    
    return mix(
        mix(
            dot(hash2(i + vec2(0, 0)), p - i - vec2(0, 0)),
            dot(hash2(i + vec2(1, 0)), p - i - vec2(1, 0)),
            t[0]),
        mix(
            dot(hash2(i + vec2(0, 1)), p - i - vec2(0, 1)),
            dot(hash2(i + vec2(1, 1)), p - i - vec2(1, 1)),
            t[0]),
        t[1]);
}

vec2 worley_hash2( in vec2 f ) { 
    float n = f.x+131.1*f.y;
    vec2 h =  fract(sin(vec2(n,n+113.0))*43758.5453123); 
    // Domain: [0, 1]^2.
    return h;
}

float worley(vec2 p) {
    vec2 c = floor(p) - vec2(1, 1);
    float min_dst = sqrt(2.f);
    for(int j = 0; j < 3; ++j)
    for(int i = 0; i < 3; ++i) {
        vec2 sc = c + vec2(i, j);
        vec2 seed = sc + worley_hash2(sc);
        min_dst = min(min_dst, distance(seed, p));
    }
    return min_dst / sqrt(2.f);
}

float terrain(vec2 p) {
    // return 64.f * (1.f - worley(p / 128.f));
    float n = 64.f * gradient_noise2(p / 128.f);
    n += 32.f * gradient_noise2(p / 64.f);
    n += 16.f * gradient_noise2(p / 32.f);
    n += 8.f * gradient_noise2(p / 16.f);
    n += 4.f * gradient_noise2(p / 8.f);
    n = max(n, 0);
    n *= worley(p / 128.f);
    return 4 * n;
}

vec3 gradient(vec2 p, float e) {
    float dx = (terrain(p + vec2(e, 0)) - terrain(p - vec2(e, 0))) / (2.f * e);
    float dy = (terrain(p + vec2(0, e)) - terrain(p - vec2(0, e))) / (2.f * e);
    return vec3(dx, dy, 0.f);
}

float slope_radians(vec2 p, float e) {
    vec3 g = gradient(p, e);
    float s = length(g);
    return atan(s);
}

void main() {
    vec2 coords = gl_FragCoord.xy / 4.f;

    float h = terrain(coords);
    fragment_height = h;

    float slope_0_1 = abs(slope_radians(coords, 1.f) / (pi / 2.f));

    vec3 grass = vec3(51.f, 204.f, 51.f) / 255.f;
    vec3 rock = vec3(128.f, 128.f, 128.f) / 255.f;
    vec3 sand = vec3(243.f, 227.f, 120.f) / 255.f;
    vec3 snow = vec3(1.f);
    vec3 water = vec3(102.f, 178.f, 255.f) / 255.f;

    vec3 color;

    if(h <= 0) {
        color = water;
        fragment_vegetation = 0.f;
    } else {
        color =
            mix(
                mix(
                    sand,
                    mix(
                        grass,
                        rock,
                        smoothstep(0.6f, 0.8f, slope_0_1)),
                    smoothstep(4.f * 2.f, 4.f * 4.f, h + 5.f * slope_0_1)),
            snow,
            smoothstep(4.f * 25.f, 4.f * 30.f, h - 4.f * 10.f * slope_0_1));

        fragment_vegetation =
           mix(
               mix(
                   0.f,
                   mix(
                       1.f,
                       0.f,
                       smoothstep(0.6f, 0.8f, slope_0_1)),
                   smoothstep(4.f * 2.f, 4.f * 4.f, h + 5.f * slope_0_1)),
           0.f,
           smoothstep(4.f * 25.f, 4.f * 30.f, h - 4.f * 10.f * slope_0_1));
    }

    fragment_color = vec4(color, 1.f);

    vec3 g = gradient(coords, 1.f);
    fragment_normal = normalize(vec3(-g.x, 1, -g.y));
}

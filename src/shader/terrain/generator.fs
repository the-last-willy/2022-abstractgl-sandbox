#version 460 core

layout(location = 0) out vec4 fragment_color;
layout(location = 1) out float fragment_height;
layout(location = 2) out vec3 fragment_normal;

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

float terrain(vec2 p) {
    float n = 64.f * gradient_noise2(p / 128.f);
    n += 32.f * gradient_noise2(p / 64.f);
    n += 16.f * gradient_noise2(p / 32.f);
    n += 8.f * gradient_noise2(p / 16.f);
    n += 4.f * gradient_noise2(p / 8.f);
    n = max(n, 0);
    return n;
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
    float h = terrain(gl_FragCoord.xy);
    fragment_height = h;

    float slope_0_1 = abs(slope_radians(gl_FragCoord.xy, 1.f) / (pi / 2.f));

    vec3 grass = vec3(51.f, 204.f, 51.f) / 255.f;
    vec3 rock = vec3(128.f, 128.f, 128.f) / 255.f;
    vec3 sand = vec3(243.f, 227.f, 120.f) / 255.f;
    vec3 snow = vec3(1.f);
    vec3 water = vec3(102.f, 178.f, 255.f) / 255.f;

    vec3 color;

    if(h <= 0) {
        color = water;
    } else {
        color =
            mix(
                mix(
                    sand,
                    mix(
                        grass,
                        rock,
                        smoothstep(0.4f, 0.5f, slope_0_1)),
                    smoothstep(2.f, 4.f, h + 5.f * slope_0_1)),
            snow,
            smoothstep(25.f, 30.f, h - 10.f * slope_0_1));
    }

    fragment_color = vec4(color, 1.f);

    vec3 g = gradient(gl_FragCoord.xy, 1.f);
    fragment_normal = normalize(vec3(-g.x, 1, -g.y));
}

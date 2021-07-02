#version 110

varying vec3 color;

uniform mat4 camera;

const vec2 dimensions = vec2(1280, 720);

vec3 light_direction() {
    return normalize(vec3(-1.f, -1.f, 0.f));    
}

// Signed distance functions.

float distance_to_cube(vec3 position) {
    vec3 d = abs(position) - vec3(0.5f);
    return length(max(d,0.0)) + min(max(max(d.x, d.y), d.z), 0.0f);
}

float distance_to_ground(vec3 position) {
    return position.y;
}

float distance_to_sphere(vec3 position) {
    return length(position) - 1.f;
}

// Objects.

vec4 ball_color(vec3 position) {
    return vec4(0.f, 0.f, 0.8f, 1.f);
}

float ball_distance(vec3 position) {
    vec3 translation = vec3(0.f, 0.5f, 5.f);
    return distance_to_sphere(position - translation);
}

vec4 box_color(vec3 position) {
    return vec4(0.f, 0.8f, 0.f, 1.f);
}

float box_distance(vec3 position) {
    vec3 translation = vec3(-2.f, 0.5f, 5.f);
    return distance_to_cube(position - translation);
}

vec4 ground_color(vec3 position) {
    bool parity = bool(int(mod(position.x, 2) >= 1) + int(mod(position.z, 2) >= 1) - 1);
    return parity ? vec4(vec3(0.8f, 0.4f, 0.4f), 1.f) : vec4(vec3(1.f, 0.8f, 0.6f), 1.f);
}

float ground_distance(vec3 position) {
    return distance_to_ground(position);
}

// Scene.

vec4 scene_color(vec3 position) {
    vec4 color;
    float min_dist = 1e6;
    { // Ball.
        float d = ball_distance(position);
        if(d < min_dist) {
            color = ball_color(position);
            min_dist = d;
        }
    }
    { // Box.
        float d = box_distance(position);
        if(d < min_dist) {
            color = box_color(position);
            min_dist = d;
        }
    }
    { // Ground.
        float d = ground_distance(position);
        if(d < min_dist) {
            color = ground_color(position);
            min_dist = d;
        }
    }
    return color;
}

float scene_distance(vec3 position) {
    float d = 1e6;
    d = min(d, ball_distance(position));
    d = min(d, box_distance(position));
    d = min(d, ground_distance(position));
    return d;
}

vec3 scene_normal(vec3 position) {
    float e = 1e-3;
    float x = scene_distance(position + vec3(  e, 0.f, 0.f)) 
            - scene_distance(position - vec3(  e, 0.f, 0.f));
    float y = scene_distance(position + vec3(0.f,   e, 0.f)) 
            - scene_distance(position - vec3(0.f,   e, 0.f));
    float z = scene_distance(position + vec3(0.f, 0.f,   e)) 
            - scene_distance(position - vec3(0.f, 0.f,   e));
    return normalize(vec3(x, y, z));
}

// Main.

void main() {
    vec2 coords = (2.f * gl_FragCoord.xy - dimensions) / dimensions.x;

    vec3 position = (camera * vec4(vec3(0.f), 1.f)).xyz;
    vec3 direction = normalize(camera * vec4(coords, 1.f, 0.f)).xyz;

    vec4 color = vec4(0.f);

    for(int i = 0; i < 500; ++i) {
        float d = scene_distance(position);
        if(d < 0.001f) {
            color = scene_color(position);
            break;
        }
        position += d * direction;
    }

    vec3 shadow_position = position - 0.02f * direction;
    float shadow_min_dist = 1e6;
    for(int i = 0; i < 500; ++i) {
        float d = scene_distance(shadow_position);
        shadow_min_dist = min(shadow_min_dist, d);
        if(d < 0.001f) {
            break;
        }
        shadow_position += d * -light_direction();
    }

    float occlusion = (distance(position, shadow_position) < 10.f) ? 1.f : 0.f;
    color.xyz *= (1.f - occlusion / 2.f);

    vec3 normal = scene_normal(position);
    float exposition = max(dot(normal, -light_direction()), 0.f);
    color.xyz *= (exposition / 2.f + 0.5f);

    if(occlusion <= 0.f) {
        vec3 light_reflection = reflect(light_direction(), normal);
        float specular = pow(max(dot(light_reflection, -direction), 0.f), 10.f);
        color.xyz = mix(color.xyz, vec3(1.f), specular);
    }

    gl_FragColor = vec4(color.xyz, 1.0);
};

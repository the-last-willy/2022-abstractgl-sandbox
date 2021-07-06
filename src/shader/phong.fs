#version 460

uniform vec3 camera_pos;
uniform vec3 light_dir;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    float shininess;
    vec3 specular;
};

uniform Material material;

in vec3 vertex_normal;
in vec3 vertex_position;

out vec4 fragment_color;

void main() {
    Light light;
    light.ambient = vec3(0.2f);
    light.diffuse = vec3(0.8f);

    

    float lambertian = max(dot(vertex_normal, -light_dir), 0.f);

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * lambertian * material.diffuse;

    vec3 view_dir = normalize(vertex_position - camera_pos);
    float spec_lambertian = max(dot(view_dir, reflect(-light_dir, vertex_normal)), 0.f);
    vec3 specular = pow(spec_lambertian, material.shininess) * vec3(1.f);

    fragment_color = vec4(ambient + diffuse + specular, 1.f);
}
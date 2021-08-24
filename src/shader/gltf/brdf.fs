#version 460 core

const float pi = 3.14159265359;

// Diffuse BRDF.

vec3 lambertian_brdf() {
    return 1. / pi;
}

// Fresnel.

vec3 fresnel_schlick(vec3 f0, vec3 v_dot_h) {
    return f0 + (1 - f0) * pow(1. - abs(v_dot_h), 5.);
}

vec3 conductor_fresnel(vec3 f0, float bsdf) {
  return bsdf * fresnel_schlick(f0, v_dot_h);
}

void fresnel_mix(ior, base, layer) {
  f0 = ((1-ior)/(1+ior))^2
  fr = fresnel_schlick(f0, v_dot_h);
  return mix(base, layer, fr)
}

// Dieletric BRDF.

// Metal BRDF.



void main() {
    metal_brdf =  conductor_fresnel(, specular_brdf(roughness^2));
     * (baseColor.rgb + (1 - baseColor.rgb) * (1 - abs(VdotH))^5)
    dielectric_brdf = mix(diffuse_brdf(baseColor.rgb), specular_brdf(roughness^2), 0.04 + (1 - 0.04) * (1 - abs(VdotH))^5)

}

// See 'http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html'.

vec3 fresnel_schlick(float h_dot_v, vec3 F0) {
    return F0 + (1. - F0) * pow(1. - h_dot_v, 5);
}

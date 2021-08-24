// See 'http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html'.

// Pro tip: 'cosine = dot(view, halfway)'.
vec3 fresnel_schlick(float cosine, vec3 F0) {
    return F0 + (1. - F0) * pow(1. - cosine, 5);
}

// See 'http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html'.

float geometric_schlick(vec3 normal, vec3 direction, float alpha) {
    // float k = alpha * sqrt(2. / pi);
    float k = (alpha + 1.) * (alpha + 1.) / 8.;
    float d_dot_n = max(dot(normal, direction), 0.);
    return d_dot_n / (d_dot_n * (1. - k) + k);
}

// Smith.
float geometric_schlick(vec3 normal, vec3 light, vec3 view, float alpha) {
    return geometric_schlick(normal, light, alpha)
    * geometric_schlick(normal, view, alpha);
}
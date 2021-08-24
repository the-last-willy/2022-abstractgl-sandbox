// See 'http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html'.

float ndf_ggx(vec3 normal, vec3 halfway, float alpha) {
    float a2 = alpha * alpha;
    float h_dot_n = max(dot(normal, halfway), 0.);
    float D0 = h_dot_n * h_dot_n * (a2 - 1.) + 1.;
    return a2 / (pi * D0 * D0);
}

// See 'http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html'.

float ndf_ggx(float h_dot_n, float alpha) {
    float a2 = alpha * alpha;
    float D0 = h_dot_n * h_dot_n * (a2 - 1.) + 1.;
    return a2 / (pi * D0 * D0);
}

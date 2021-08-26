// See 'http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html'.

float geometric_schlick(float n_dot_lv, float alpha) {
    // float k = alpha * sqrt(2. / pi);
    float k = (alpha + 1.) * (alpha + 1.) / 8.;
    return n_dot_lv / (n_dot_lv * (1. - k) + k);
}

// Smith.
float geometric_schlick(float n_dot_l, float n_dot_v, float alpha) {
    return geometric_schlick(n_dot_l, alpha)
    * geometric_schlick(n_dot_v, alpha);
}
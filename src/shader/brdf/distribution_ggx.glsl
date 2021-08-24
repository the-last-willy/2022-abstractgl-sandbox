float distribution_ggx(vec3 n, vec3 h, float a) {
    float alpha = a * a;
    float h_dot_n = dot(h, n);
    float D0 = h_dot_n * h_dot_n * (alpha - 1) + 1;
    return alpha / (pi * D0 * D0);
}

float line_sdf(vec3 a, vec3 b, vec3 p) {
    vec3 ab = b - a, ap = p - a;
    vec3 q = a + dot(ap, ab) / dot(ab, ab) * ab;
    return length(q - ap);
}

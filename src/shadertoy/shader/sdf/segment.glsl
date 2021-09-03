float segment_sdf(vec3 a, vec3 b, vec3 p) {
    vec3 ab = b - a, ap = p - a;
	float h = clamp(dot(ab, ap) / dot(ab, ab), 0., 1.);
	return length(ap - h * ab);
}

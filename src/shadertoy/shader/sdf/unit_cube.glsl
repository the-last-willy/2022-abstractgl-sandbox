float unit_cube_sdf(vec3 p) {
    p = abs(p);
    float exterior = length(max(p - vec3(.5), 0.));
    float interior = min(max(p.x, max(p.y, p.z)) - .5, 0.);
    return exterior + interior;
}

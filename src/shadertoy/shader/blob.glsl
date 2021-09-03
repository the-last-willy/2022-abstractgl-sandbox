#include "shadertoy/shader/shadertoy_prefix.fs"

#include "shadertoy/shader/sdf/all.glsl"

uniform mat4 view;

// Common

// Hashing function
// Returns a random number in [-1,1]
float Hash(float seed) {
    return fract(sin(seed) * 43758.5453);
}

// Cosine direction
vec3 Cosine(in float seed, in vec3 nor) {
    float u = Hash(78.233 + seed);
    float v = Hash(10.873 + seed);

    // method 3 by fizzer: http://www.amietia.com/lambertnotangent.html
    float a = 6.2831853 * v;
    u = 2. * u - 1.;
    return normalize(nor + vec3(sqrt(1. - u * u) * vec2(cos(a), sin(a)), u));
}

// Rotation matrix around z axis
// a : Angle
mat3 rotate_z(float a) {
    float sa = sin(a);
    float ca = cos(a);
    return mat3(
         ca, sa, 0.,
        -sa, ca, 0., 
         0., 0., 1.);
}

// Compute the ray
// m : Mouse position
// p : Pixel
// ro, rd : Ray origin and direction
void Ray(in vec2 m, in vec2 p, out vec3 ro, out vec3 rd) {
    float a = 3. * 3.14 * m.x;
    float le = 3.8;

    ro = vec3(20., 0., 5.);
    ro *= rotate_z(3. * 3.14 * m.x);

    vec3 ta = vec3(0., 0., 1.);
    vec3 ww = normalize(ta - ro);
    vec3 uu = normalize(cross(ww, vec3(0., 0., 1.)));
    vec3 vv = normalize(cross(uu, ww));
    rd = normalize(p.x * uu + p.y * vv + le * ww);
}

// Main

const int Steps = 1000;
const float Epsilon = .03; // Marching epsilon
const float T = .5;

const float rA = 10.; // Maximum and minimum ray marching or sphere tracing distance from origin
const float rB = 40.;

// Transforms
vec3 rotateX(vec3 p, float a) {
    float sa = sin(a);
    float ca = cos(a);
    return vec3(p.x, ca * p.y - sa * p.z, sa * p.y + ca * p.z);
}

vec3 rotateY(vec3 p, float a) {
    float sa = sin(a);
    float ca = cos(a);
    return vec3(ca * p.x + sa * p.z, p.y, -sa * p.x + ca * p.z);
}

vec3 rotateZ(vec3 p, float a) {
    float sa = sin(a);
    float ca = cos(a);
    return vec3(ca * p.x + sa * p.y, -sa * p.x + ca * p.y, p.z);
}

// Smooth cubic falloff function
// x : distance
// R : radius
float falloff(float x, float R) {
    float u = clamp(x / R, 0., 1.);
    float v = (1. - u * u);
    return v * v * v;
}

// Primitives

// Point skeleton
// p : point
// c : center of skeleton
// e : energy associated to skeleton
// R : radius
float point(vec3 p, vec3 c, float e, float R) {
    return e * falloff(length(p - c), R);
}

// Operators

// Blending
// a,b : field function of the sub-trees
float Blend(float a, float b) {
    return a + b;
}

// Union
// a : field function of left sub-tree
// b : field function of right sub-tree
float Union(float a, float b) {
    return max(a, b);
}

// Potential field of the object
// p : point
float Object(vec3 p) {
    p.z = -p.z;
    // float v = Blend(point(p, vec3(0., 1., 1.), 1., 4.5),
    //                 point(p, vec3(2., 0., -3.), 1., 4.5));

    // v = Blend(v, point(p, vec3(-3., 2., -3.), 1., 4.5));
    // v = Union(v, point(p, vec3(-1., -1., 0.), 1., 4.5));

    float v = 0.f;
    // float v = Union(0.5-point_sdf(-vec3(1.), p), 0.5-point_sdf(vec3(1.), p));
    v = Union(v, 0.1 - unit_cube_sdf(p / 2.));

    return v;
}

// Calculate object normal
// p : point
vec3 ObjectNormal(in vec3 p) {
    float eps = .0001;
    vec3 n;
    float v = Object(p);
    n.x = Object(vec3(p.x + eps, p.y, p.z)) - v;
    n.y = Object(vec3(p.x, p.y + eps, p.z)) - v;
    n.z = Object(vec3(p.x, p.y, p.z + eps)) - v;
    return normalize(n);
}

// Calculate object normal
// p : point
vec3 ObjectNormal6(in vec3 p) {
    float e = .0001;
    float nx = Object(vec3(p.x + e, p.y, p.z)) - Object(vec3(p.x - e, p.y, p.z));
    float ny = Object(vec3(p.x, p.y + e, p.z)) - Object(vec3(p.x, p.y - e, p.z));
    float nz = Object(vec3(p.x, p.y, p.z + e)) - Object(vec3(p.x, p.y, p.z - e));
    return normalize(vec3(nx, ny, nz));
}

// Trace ray using ray marching
// o : ray origin
// u : ray direction
// h : hit
// s : Number of steps
float Trace(vec3 o, vec3 u, float rB, out bool h, out int s) {
    h = false;

    // Don't start at the origin, instead move a little bit forward
    float t = rA;

    for(int i = 0; i < Steps; i++) {
        s = i;
        vec3 p = o + t * u;
        float v = Object(p);
        // Hit object
        if(v > 0.) {
            s = i;
            h = true;
            break;
        }
        // Move along ray
        t += Epsilon;
        // Escape marched far away
        if(t > rB) {
            break;
        }
    }
    return t;
}

// Trace ray using ray marching
// o : ray origin
// u : ray direction
// h : hit
// s : Number of steps
float SphereTrace(vec3 o, vec3 u, float rB, out bool h, out int s) {
    h = false;

    // Don't start at the origin, instead move a little bit forward
    float t = rA;

    for (int i = 0; i < Steps; i++)
    {
        s = i;
        vec3 p = o + t * u;
        float v = Object(p);
        // Hit object
        if (v > 0.)
        {
            s = i;
            h = true;
            break;
        }
        // Move along ray
        t += max(Epsilon, abs(v) / 4.);
        // Escape marched far away
        if (t > rB)
        {
            break;
        }
    }
    return t;
}

// Ambient occlusion
// p : Point
// n : Normal
// a : Number of smaples
float AmbientOcclusion(vec3 p, vec3 n, int a) {
    if (a == 0)
    {
        return 1.;
    }

    float ao = 0.;

    for (int i = 0; i < a; i++)
    {
        vec3 d = Cosine(581.123 * float(i), n);

        int s;
        bool h;
        float t = SphereTrace(p, d, 10., h, s);
        if (!h)
        {
            ao += 1.;
        }
        else if (t > 5.)
        {
            ao += 1.;
        }
    }

    ao /= float(a);
    return ao;
}

// Background color
vec3 background(vec3 rd) {
    return mix(vec3(.652, .451, .995), vec3(.552, .897, .995), rd.z * .5 + .5);
}

float Light(vec3 p, vec3 n) {
    // point light
    const vec3 lp = vec3(5., 10., 25.);

    vec3 l = normalize(lp - p);

    // Not even Phong shading, use weighted cosine instead for smooth transitions
    float diff = pow(.5 * (1. + dot(n, l)), 2.);

    bool h;
    int s;
    float t = SphereTrace(p + .1 * n, l, 100., h, s);
    if(!h) {
        return diff;
    }
    return 0.;
}

float SmoothLight(vec3 p, vec3 n, int a) {
    if(a == 0)
        return 1.;

    // point light
    const vec3 lp = vec3(5., 10., 25.);

    vec3 l = normalize(lp - p);

    float lo = 0.;

    for(int i = 0; i < a; i++) {
        vec3 d = Cosine(581.123 * float(i), n);
        d = normalize(l + d * .15);
        int s;
        bool h;
        float t = SphereTrace(p, d, 10., h, s);
        if(!h) {
            lo += 1.;
        } else if(t > 100.) {
            lo += 1.;
        }
    }

    lo /= float(a);
    return lo;
}

// Shading and lighting
// p : point,
// n : normal at point
vec3 Shade(vec3 p, vec3 n) {
    vec3 c = .25 + .25 * background(n);
    c += .15 * AmbientOcclusion(p + .1 * n, n, 0) * vec3(1., 1., 1.);
    c += .35 * Light(p, n);
    return c;
}

// Shading with number of steps
vec3 ShadeSteps(int n) {
    float t = float(n) / (float(Steps - 1));
    return .5 + mix(vec3(.05, .05, .5), vec3(.65, .39, .65), t);
}

// Picture in picture
// pixel : Pixel
// pip : Boolean, true if pixel was in sub-picture zone
vec2 Pip(in vec2 pixel, out bool pip) {
    // Pixel coordinates
    vec2 p = (-iResolution.xy + 2. * pixel) / iResolution.y;
    if (pip == true)
    {
        const float fraction = 1. / 4.;
        // Recompute pixel coordinates in sub-picture
        if ((pixel.x < iResolution.x * fraction) && (pixel.y < iResolution.y * fraction))
        {
            p = (-iResolution.xy * fraction + 2. * pixel) / (iResolution.y * fraction);
            pip = true;
        }
        else
        {
            pip = false;
        }
    }
    return p;
}

// Image
void mainImage(out vec4 color, in vec2 pxy) {
    // Picture in picture on
    bool pip = true;

    // Pixel
    vec2 pixel = Pip(pxy, pip);

    // Mouse
    vec2 m = iMouse.xy / iResolution.xy;

    // Camera
    vec3 ro, rd;
    Ray(m, pixel, ro, rd);

    // Trace ray

    // Hit and number of steps
    bool hit;
    int s;

    float t = SphereTrace(ro, rd, 100., hit, s);

    // Position
    vec3 pt = ro + t * rd;

    // Shade background
    vec3 rgb = background(rd);

    if (hit)
    {
        // Compute normal
        vec3 n = ObjectNormal(pt);

        // Shade object with light
        rgb = Shade(pt, n);
    }

    // Uncomment this line to shade image with false colors representing the number of steps
    if (pip == true)
    {
        rgb = ShadeSteps(s);
    }

    color = vec4(rgb, 1.);
}

#include "shadertoy/shader/shadertoy_suffix.fs"

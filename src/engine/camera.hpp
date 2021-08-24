#pragma once

#include <agl/all.hpp>

#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>
#include <variant>

namespace eng {

struct OrthographicProjection {
    
};

struct PerspectiveProjection {
    float aspect_ratio = 1.f;
    float y_fov = agl::pi / 2.f;
    std::optional<float> z_far = 1'000.f;
    float z_near = 0.1f;
};

inline
auto transform(const PerspectiveProjection& op) {
    if(op.z_far) {
        auto a = op.aspect_ratio;
        auto y = op.y_fov;
        auto n = op.z_near;
        auto f = *op.z_far;

        auto tan_y = std::tan(y / 2.f);
        auto e33 = -(n + f) / (n - f);
        auto e43 = 2.f * n * f / (n - f);

        // SPECIFICATION.
        // return agl::mat4(
        //     1.f / (a * tan_y),         0.f, 0.f, 0.f,
        //                   0.f, 1.f / tan_y, 0.f, 0.f,
        //                   0.f,         0.f, e33, 1.f,
        //                   0.f,         0.f, e43, 0.f);

        // MINE USES X_FOV INSTEAD. FIX IT, YOU LAZY BUN. TODO;
        (void) a;
        (void) tan_y;
        return agl::mat4(
            1.f,         0.f, 0.f, 0.f,
                          0.f, op.aspect_ratio, 0.f, 0.f,
                          0.f,         0.f, e33, 1.f,
                          0.f,         0.f, e43, 0.f);
    } else {
        throw std::runtime_error("Not implemented infinite perspective projection.");
    }
}

struct Camera {
    agl::Mat4 transform = mat4(agl::identity);

    std::variant<OrthographicProjection, PerspectiveProjection> projection
    = PerspectiveProjection();
};

inline
auto transform(const Camera& c) {
    if(auto pp = std::get_if<PerspectiveProjection>(&c.projection)) {
        return transform(*pp) * c.transform;
    }
    throw std::runtime_error("NOT IMPLEMENTED.");
}

}

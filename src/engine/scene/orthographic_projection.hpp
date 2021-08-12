#pragma once

#include "agl/all.hpp"

namespace eng {

struct OrthographicProjection {
	float near_ = 0.1f;
	float far_ = 1000.f;

	float zoom = 1.f;

	float aspect_ratio = 16.f / 9.f;
};

constexpr
auto transform(const OrthographicProjection& p) {
	// View space -> Clip space
	// (near, far) -> (-1, 1)

	// | 1 | 0 | 0 | 0 | = x
	// | 0 | 1 | 0 | 0 | = y
	// | 0 | 0 | a | b | -> (-1, 1)
	// | 0 | 0 | 0 | 1 | = w

	auto a = - 2 / (p.near_ - p.far_);
	auto b = (p.near_ + p.far_) / (p.near_ - p.far_);

	auto x = p.zoom;
	auto y = p.zoom * p.aspect_ratio;
	return agl::mat4( // A line is a column.
		  x, 0.f,  0.f, 0.f,
		0.f,   y,  0.f, 0.f,
		0.f, 0.f,    a, 0.f,
		0.f, 0.f,    b, 1.f);
}

}

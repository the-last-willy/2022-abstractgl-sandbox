#pragma once

#include "agl/all.hpp"

namespace tlw {

struct PerspectiveProjection {
	float aspect_ratio = 16.f / 9.f;

	float near = 0.1f;
	float far = 1000.f;
};

constexpr
auto transform(const PerspectiveProjection& p) {
	// View space -> Clip space
	// (near, far) -> (-near, far)

	// | 1 | 0 | 0 | 0 | = x
	// | 0 | 1 | 0 | 0 | = y
	// | 0 | 0 | a | b | -> (-near, far)
	// | 0 | 0 | 1 | 0 | = z

	auto a = (p.far + p.near) / (p.far - p.near);
	auto b = -2 * p.far * p.near / (p.far - p.near);

	auto y = p.aspect_ratio;
	return agl::mat4( // A line is a column.
		1.f, 0.f,  0.f, 0.f,
		0.f,   y,  0.f, 0.f,
		0.f, 0.f,    a, 1.f,
		0.f, 0.f,    b, 0.f);
}

}

#pragma once

#include "matrix.hpp"

#include "gl/vector/all.hpp"

namespace tlw::gl {

constexpr
auto lookat(Vec3 forward, Vec3 up) {
	// TODO: fix.
	auto right = cross(forward, up);
	auto up2 = cross(right, forward);
	return Mat4{
		right[0], right[1], right[2], 0.f,
		up2[0], up2[1], up2[2], 0.f,
		forward[0], forward[1], forward[2], 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

}

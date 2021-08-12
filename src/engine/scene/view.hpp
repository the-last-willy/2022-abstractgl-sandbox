#pragma once

#include "agl/all.hpp"

namespace tlw {

struct View {
	agl::Vec3 rotation_center = agl::vec3(0.f, 0.f, 0.f);
	agl::Vec3 position = agl::vec3(0.f, 0.f, 0.f);

	float zoom = 1.f;

	// Aircraft rotation.
	float yaw = 0.f;
	float pitch = 0.f;
	float roll = 0.f;
};

constexpr
auto rotation(const View& v) {
	return agl::rotation_y(v.yaw)
	* agl::rotation_x(v.pitch)
	* agl::rotation_z(v.roll);
}

constexpr
auto transform(const View& v) {
	return agl::translation(v.position)
	* agl::scaling3(v.zoom)
	* agl::rotation_y(v.yaw)
	* agl::rotation_x(v.pitch)
	* agl::rotation_z(v.roll)
	* agl::translation(v.rotation_center);
}

}

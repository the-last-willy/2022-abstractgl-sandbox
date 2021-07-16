#pragma once

namespace tlw {

struct View {
	Vec3 rotation_center = Vec3{0.f, 0.f, 0.f};
	Vec3 position = Vec3{0.f, 0.f, 0.f};

	// Aircraft rotation.
	float yaw = 0.f;
	float pitch = 0.f;
};

constexpr
auto transform(const View& v) {
	return translation(v.position)
	       * rotation_y(v.yaw)
	       * rotation_x(v.pitch)
	       * translation(v.rotation_center);
}

}

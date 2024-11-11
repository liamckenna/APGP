#include "color.h"
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
bool Color::NameMatch(COLORS color_name) {
	return (r == ((color_name >> 16) & 0xFF) &&
			g == ((color_name >> 8) & 0xFF) &&
			b ==  (color_name & 0xFF));
}

bool Color::operator==(Color c) {
	if (this->r == c.r &&
		this->g == c.g &&
		this->b == c.b &&
		this->a == c.a) return true;
	else return false;
};

glm::vec4 Color::ToVec4() {
	return glm::vec4(
		rgba[0] / 255.f,
		rgba[1] / 255.f,
		rgba[2] / 255.f,
		rgba[3] / 255.f
	);
}

glm::vec3 Color::ToVec3() {
	return glm::vec3(
		rgba[0] / 255.f,
		rgba[1] / 255.f,
		rgba[2] / 255.f
	);
}

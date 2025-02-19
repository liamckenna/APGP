#pragma once
#include <glm/glm.hpp>

struct FlatLight {
	glm::vec3 position;
	float intensity;
	glm::vec3 color;
	int enabled;
};
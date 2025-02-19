#pragma once
#include <glm/glm.hpp>

struct FlatLight {
	int enabled;
	int index;
	float intensity;
	float pad1;

	glm::vec3 position;
	float pad2;

	glm::vec3 color;
	float pad3;
};
#pragma once
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

struct FlatVertex {
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
	glm::vec3 tangent{ 1.f, 0.f, 0.f };
};
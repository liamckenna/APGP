#pragma once

#include <glm/vec3.hpp>

struct Normal {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		glm::vec3 n; //normal vector
	};

	Normal(float x = 0.f, float y = 0.f, float z = 0.f);
	Normal(glm::vec3 n);

	float& operator[](int index);
};

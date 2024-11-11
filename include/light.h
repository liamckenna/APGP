#pragma once
#include "transform.h"
#include "color.h"
#include "object.h"
struct Light : public Object {

	float strength;
	Color c;

	
	Light(Transform* t, float s = 100.f, Color c = Color(), bool a = true);
	Light(float p0, float p1, float p2, float s = 100.f, Color c = Color(), bool a = true);

	void UpdateSelf(bool rendering) override {
		
	}
};

struct FlattenedLight {
	glm::vec3 position;		//12 bytes, aligned to 16 bytes
	float strength;			//4 bytes
	glm::vec3 color;		//12 bytes, aligned to 16 bytes
	int active;				//stored as int (4 bytes) in C++
};
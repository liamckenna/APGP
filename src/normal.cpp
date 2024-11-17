#include "normal.h"

Normal::Normal(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Normal::Normal(glm::vec3 n) {
	this->n = n;
}

float& Normal::operator[](int index) {
	return n[index];
}
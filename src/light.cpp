#include "light.h"
#include "color.h"
Light::Light() {
	t = new Transform(glm::vec3(0.f, 2.f, 0.f), this);
	strength = 1.f;
	c = Color(WHITE);
}

Light::Light(Transform* t, float s, Color c, bool a) : Object(t) {
	object_type = LIGHT;
	strength = s;
	this->c = c;
	active_local = a;
}

Light::Light(float p0, float p1, float p2, float s, Color c, bool a) {
	object_type = LIGHT;
	this->t = new Transform(glm::vec3(p0, p1, p2), this);
	strength = s;
	this->c = c;
	active_local = a;
}
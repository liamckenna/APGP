#include "light.h"
#include "lights.h"
#include "color.h"
#include "json.h"
Light::Light() {
	t = new Transform(glm::vec3(0.f, 2.f, 0.f), this);
	strength = 1.f;
	c = Color(WHITE);
}

Light::Light(const nlohmann::json& data, Scene* scene) {
	this->scene = scene;
	object_type = LIGHT;
	if (data.contains("transform")) t = new Transform(data["transform"], this);
	else t = new Transform(glm::vec3(0.f, 2.f, 0.f), this);
	strength = Fetch(data, "strength", 1.f);
	if (data.contains("color")) c = Color(data["color"][0], data["color"][1], data["color"][2]);
	else c = Color(WHITE);
	//name
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
#include "lights.h"
#include <iostream>
#include "util.h"

Lights::Lights() {
	it = 0;
	dval = new Light();
	current = dval;
	lights.push_back(dval);
}

Lights::Lights(const nlohmann::json& data, Scene* scene) {

	std::cout << "lights obj created" << std::endl;

	it = 0;
	this->scene = scene;
	
	InsertDefault();

	std::cout << "default inserted" << std::endl;
	std::cout << "lights size: " << data.size() << std::endl;
	for (int i = 0; i < data.size(); i++) {
		Insert(data[i]);
		std::cout << "INSERTING LIGHT " << (i + 1) << std::endl;
	}

	SetupBuffer();

	std::cout << "light generation completed" << std::endl;
}

void Lights::SetupBuffer() {

	GenerateBuffer();
	PopulateBuffer(Flatten());
	BindBuffer();

}

void Lights::GenerateBuffer() {
	glGenBuffers(1, &ubo);
	binding_point = LIGHT_BINDING_POINT;
}

void Lights::PopulateBuffer(std::vector<FlattenedLight> flattened_lights) {

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(FlattenedLight) * lights.size(), flattened_lights.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void Lights::BindBuffer() {

	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo); 

}


void Lights::InsertDefault() {
	dval = new Light();
	current = dval;
	lights.push_back(dval);
	dval->idx = IterateIndex();
	dval->scene = scene;
}

void Lights::Insert(const nlohmann::json& data) {
	std::cout << "at Insert, in = " << it << std::endl;
	Light* light = new Light(data, scene);
	light->idx = IterateIndex();
	lights.push_back(light);
}

int Lights::IterateIndex() {
	int idx = it;
	it += 1;
	std::cout << "returning " << idx << std::endl;
	return idx;
}

std::vector<FlattenedLight> Lights::Flatten() {
	std::vector<FlattenedLight> flattened_lights;

	for (const auto& light : this->lights) {
		FlattenedLight flattened_light;
		flattened_light.position = light->t->global.pos;
		flattened_light.strength = light->strength;
		flattened_light.color = light->c.ToVec3();
		flattened_light.active = (light->active_local && light->active_global);
		flattened_lights.push_back(flattened_light);
	}
	return flattened_lights;
}

Light* Lights::GetByName(std::string name) {

	for (auto& light : lights) {
		if (light->name == name) return light;
	}
	return nullptr;

}


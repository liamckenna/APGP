#pragma once

#include "light.h"
#include <vector>
#include "json.h"

struct Lights {

	std::vector<Light*> lights;
	Scene* scene;
	Light* dval;
	Light* current;

	int it;

	GLuint ubo;
	GLuint binding_point;

	Lights();
	Lights(const nlohmann::json& data, Scene* scene);

	void SetupBuffer();
	void GenerateBuffer();
	void PopulateBuffer(std::vector<FlattenedLight> flattened_lights);
	void BindBuffer();

	std::vector<FlattenedLight> Flatten();

	Light* GetByName(std::string name);
	int IterateIndex();
	void InsertDefault();
	void Insert(const nlohmann::json& data);
};
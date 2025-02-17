#pragma once

#include "camera.h"
#include <vector>
#include "json.h"
struct Cameras {

	std::vector<Camera*> cameras;
	Scene* scene;
	Camera* dval;
	Camera* current;
	int it;
	Cameras();

	Cameras(const nlohmann::json& data, Scene* scene);

	Camera* GetByName(std::string name);
	int IterateIndex();
	void InsertDefault();
	void Insert(const nlohmann::json& data);
};
#pragma once

#include <vector>
#include "mesh.h"

struct Meshes {
	std::vector<Mesh*> meshes;
	Scene* scene;
	Mesh* dval; //make cube for default scene
	Mesh* current;
	int it;
	Meshes();
	Meshes(const nlohmann::json& data, Scene* scene);
	Mesh* GetByName(std::string name);
	int IterateIndex();
	void InsertDefault();
	void Insert(const nlohmann::json& data);
};
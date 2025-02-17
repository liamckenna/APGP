#pragma once

#include <vector>
#include "object.h"

struct Objects {
	std::vector<Object*> objects;
	Scene* scene;
	Object* dval; //make cube for default scene
	Object* current;
	int it;
	Objects();
	Objects(const nlohmann::json& data, Scene* scene);
	Object* GetByName(std::string name);
	int IterateIndex();
	void InsertDefault();
	void Insert(const nlohmann::json& data);
};
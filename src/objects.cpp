#include "objects.h"

Objects::Objects() {
	scene = nullptr;
	InsertDefault();
}

Objects::Objects(const nlohmann::json& data, Scene* scene) {
	it = 0;
	this->scene = scene;
	
	InsertDefault();


	for (int i = 0; i < data.size(); i++) {
		Insert(data[i]);
	}

}

void Objects::InsertDefault() {
	dval = new Object();
	current = dval;
	objects.push_back(dval);
	dval->idx = IterateIndex();
	dval->scene = scene;
}

void Objects::Insert(const nlohmann::json& data) {

	Object* object = new Object(data, scene);
	objects.push_back(object);
	object->idx = IterateIndex();
}

int Objects::IterateIndex() {
	int idx = it;
	it++;
	return idx;
}

Object* Objects::GetByName(std::string name) {

	for (int i = 0; i < objects.size(); i++) {
		if (objects[i]->name == name) return objects[i];
	}
	return nullptr;
}
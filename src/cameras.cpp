#include "cameras.h"

Cameras::Cameras() {
	it = 0;
	dval = new Camera();
	current = dval;
	cameras.push_back(dval);
}

Cameras::Cameras(const nlohmann::json& data, Scene* scene) {
	it = 0;
	this->scene = scene;
	
	InsertDefault();

	for (int i = 0; i < data.size(); i++) {
		Insert(data[i]);
	}

	std::cout << "camera generation completed" << std::endl;
}

void Cameras::InsertDefault() {
	dval = new Camera();
	current = dval;
	cameras.push_back(dval);
	dval->idx = IterateIndex();
	dval->scene = scene;
}

void Cameras::Insert(const nlohmann::json& data) {
	
	Camera* camera = new Camera(data, scene);
	cameras.push_back(camera);
	camera->idx = IterateIndex();
}

int Cameras::IterateIndex() {
	int idx = it;
	it++;
	return idx;
}

Camera* Cameras::GetByName(std::string name) {

	for (auto& camera : cameras) {
		if (camera->name == name) return camera;
	}
	return nullptr;

}
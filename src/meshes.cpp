#include "meshes.h"

Meshes::Meshes() {
	dval = new Mesh();
	current = dval;
	meshes.push_back(dval);
}

Meshes::Meshes(const nlohmann::json& data, Scene* scene) {
	it = 0;
	this->scene = scene;
	
	InsertDefault();

	for (int i = 0; i < data.size(); i++) {
		Insert(data[i]);
	}

	std::cout << "mesh generation completed" << std::endl;

}


void Meshes::InsertDefault() {
	dval = new Mesh();
	current = dval;
	meshes.push_back(dval);
	dval->idx = IterateIndex();
	dval->scene = scene;
}

void Meshes::Insert(const nlohmann::json& data) {

	Mesh* mesh = new Mesh(data, scene);
	meshes.push_back(mesh);
	mesh->idx = IterateIndex();
}

int Meshes::IterateIndex() {
	int idx = it;
	it++;
	return idx;
}

Mesh* Meshes::GetByName(std::string name) {
	
	for (auto& mesh : meshes) {
		if (mesh->name == name) return mesh;
	}
	return nullptr;

}
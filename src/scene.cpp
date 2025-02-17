#include "scene.h"
#include "mesh.h"
#include "meshes.h"
#include "camera.h"
#include "cameras.h"
#include "light.h"
#include "lights.h"
#include "shaders.h"
#include "material.h"
#include "materials.h"
#include "texture.h"
#include "textures.h"
#include "object.h"
#include "objects.h"
#include <iostream>
#include <cctype>
#include "program.h"

Scene::Scene(const std::string& filepath, Program* program) {

	this->program = program;
	user = program->user;

	nlohmann::json data = ReadJsonFromFile(filepath);

	name = Fetch(data, "name", "My Scene");

	std::cout << "working on lights" << std::endl;
	
	lights = new Lights(data["lights"], this);
	
	std::cout << "setup lights" << std::endl;

	cameras = new Cameras(data["cameras"], this);
	
	std::cout << "setup cameras" << std::endl;

	materials = new Materials(this);
	
	std::cout << "setup materials" << std::endl;

	textures = new Textures(this);

	std::cout << "setup textures" << std::endl;
	
	meshes = new Meshes(data["meshes"], this);

	std::cout << "setup meshes" << std::endl;
	
	objects = new Objects(data["objects"], this);

	std::cout << "setup objects" << std::endl;
	
	materials->SetupBuffer();

	std::cout << "setup mat buffer" << std::endl;

	textures->SetupBuffer();

	std::cout << "setup text buffer" << std::endl;
	
	std::cout << "scene generation completed" << std::endl;


}


void Scene::PrintObjectTrees() {
	std::cout << "BEGINNING TREE" << std::endl;
	for (auto& object : objects->objects) {
		if (object->parent == nullptr) object->PrintTree(0);
	}
	std::cout << "TREE HAS CONCLUDED" << std::endl;
}

void Scene::DrawObjectTrees() {
	for (auto& object : objects->objects) {
		object->DrawTree(cameras->dval);
	}
}

void Scene::UpdateObjectTrees(bool rendering) {
	for (auto& object : objects->objects) {
		object->UpdateTree(rendering);
	}
}

void Scene::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawObjectTrees();
}
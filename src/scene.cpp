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
#include "ecs_manager.h"

Scene::Scene(const std::string& filepath, Program* program) : scene_ecs(), resource_manager() {

	this->program = program;
	user = program->user;

	nlohmann::json data = ReadJsonFromFile(filepath);

	name = Fetch(data, "name", "My Scene");

	if (!program->hardcoded) {
		//existing code
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

		std::cout << "setup material buffer" << std::endl;

		textures->SetupBuffer();

		std::cout << "setup texture buffer" << std::endl;
	}
	else {
		
		
		Entity camera_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(camera_entity, TransformComponent{ .position = glm::vec3(3.f, 3.f, 3.f) });
		scene_ecs.AddComponent(camera_entity, CameraComponent{});
		scene_ecs.AddComponent(camera_entity, PrimaryCameraComponent{});

		Entity light_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(light_entity, TransformComponent{ .position = glm::vec3(0.f, 5.f, 0.f) });
		scene_ecs.AddComponent(camera_entity, LightComponent{});

		Entity cube_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(cube_entity, TransformComponent{});
		scene_ecs.AddComponent(cube_entity, MeshComponent{.mesh_filepath = "default.obj"});
		scene_ecs.AddComponent(cube_entity, MaterialComponent{});

	}
	
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
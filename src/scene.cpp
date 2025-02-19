#include "scene.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#include <iostream>
#include <cctype>
#include "program.h"
#include "ecs_manager.h"
#include "light_system.h"
#include "render_system.h"

Scene::Scene(const std::string& filepath, Program* program) : scene_ecs(), resource_manager() {

	this->program = program;
	user = program->user;

	nlohmann::json data = ReadJsonFromFile(filepath);
	name = Fetch(data, "name", "My Scene");

	scene_ecs.AddSystem<LightSystem>();
	scene_ecs.AddSystem<RenderSystem>(resource_manager, program->shader_manager);

	if (!program->hardcoded) {}
	else {

		Entity camera_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(camera_entity, TransformComponent{ .position = glm::vec3(5.f, 3.f, 3.f) });
		scene_ecs.AddComponent(camera_entity, CameraComponent{});
		scene_ecs.AddComponent(camera_entity, PrimaryCameraComponent{});

		Entity light_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(light_entity, TransformComponent{ .position = glm::vec3(0.f, 5.f, 0.f) });
		scene_ecs.AddComponent(camera_entity, LightComponent{});

		Entity cube_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(cube_entity, TransformComponent{});
		scene_ecs.AddComponent(cube_entity, MeshComponent{.mesh_name = "default"}, resource_manager);

	}
	std::cout << "scene generation completed" << std::endl;
}

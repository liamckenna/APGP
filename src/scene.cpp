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
#include "input_system.h"
#include "window.h"

Scene::Scene(const std::string& filepath, Program& program) : scene_ecs(), resource_manager(), program(program) {

	nlohmann::json data = ReadJsonFromFile(filepath);
	name = Fetch(data, "name", "My Scene");

	
	scene_ecs.AddSystem<RenderSystem>(resource_manager, program.shader_manager);
	scene_ecs.AddSystem<LightSystem>(program.shader_manager);
	scene_ecs.AddSystem<InputSystem>(program.input_manager);

	if (!program.hardcoded) {
		
	}
	else {
		Entity default_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(default_entity, TransformComponent{ .position = glm::vec3(-7.f, 1.f, 0.f), .scale = glm::vec3(1.5f)});
		scene_ecs.AddComponent(default_entity, MeshComponent{ .mesh_name = "default" }, resource_manager);

		Entity camera_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(camera_entity, TransformComponent{.position = glm::vec3(0.f, 1.f, 0.f)});
		scene_ecs.AddComponent(camera_entity, CameraComponent{ .fov = 90.f });
		scene_ecs.AddComponent(camera_entity, PrimaryCameraComponent{});

		Entity light_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(light_entity, TransformComponent{ .position = glm::vec3(0.f, 5.f, 0.f) });
		scene_ecs.AddComponent(light_entity, LightComponent{});
		scene_ecs.AddComponent(light_entity, PointLightComponent());

		Entity pokeball_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(pokeball_entity, TransformComponent{ .position = glm::vec3(0.f, 1.f, -7.f) });
		scene_ecs.AddComponent(pokeball_entity, MeshComponent{.mesh_name = "pokeball"}, resource_manager);

		Entity floor_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(floor_entity, TransformComponent{ .scale = glm::vec3(5.f)});
		scene_ecs.AddComponent(floor_entity, MeshComponent{ .mesh_name = "floor" }, resource_manager);



	}
	std::cout << "scene generation completed" << std::endl;
}

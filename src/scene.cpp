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
#include "windows.h"

Scene::Scene(const std::string& filepath, Program& program) : scene_ecs(), resource_manager(), program(program) {

	nlohmann::json data = ReadJsonFromFile(filepath);
	name = Fetch(data, "name", "My Scene");

	scene_ecs.AddSystem<LightSystem>(program.shader_manager);
	scene_ecs.AddSystem<RenderSystem>(resource_manager, program.shader_manager);
	scene_ecs.AddSystem<InputSystem>(program.input_manager);

	if (!program.hardcoded) 
	{
		for (int i = 0; i < data["entities"].size(); i++)
		{
			Entity entity = scene_ecs.CreateEntity();
			if (data["entities"][i].contains("transform"))
			{
				glm::vec3 position = FetchVec3(data["entities"][i]["transform"], "position", glm::vec3(0.f));
				glm::vec3 rotation = FetchVec3(data["entities"][i]["transform"], "rotation", glm::vec3(0.f));
				glm::vec3 scale = FetchVec3(data["entities"][i]["transform"], "scale", glm::vec3(1.f));
				scene_ecs.AddComponent(entity, TransformComponent{ 
					.position = position, 
					.orientation = glm::quat(glm::radians(rotation)), 
					.scale = scale 
					});
			}
			if (data["entities"][i].contains("mesh"))
			{
				scene_ecs.AddComponent(entity, MeshComponent{ .mesh_name = Fetch(data["entities"][i], "mesh", "default")}, resource_manager);
			}
			if (data["entities"][i].contains("light"))
			{
				scene_ecs.AddComponent(entity, LightComponent{});
				//todo
			}
			if (data["entities"][i].contains("camera"))
			{
				float fov = Fetch(data["entities"][i]["camera"], "fov", 90.f);
				scene_ecs.AddComponent(entity, CameraComponent{ .fov = fov });
				if (Fetch(data["entities"][i]["camera"], "primary", false))
				{
					scene_ecs.AddComponent(entity, PrimaryCameraComponent{});
				}
			}
		}
	}
	else 
	{
		Entity default_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(default_entity, TransformComponent{ .position = glm::vec3(-7.f, 1.f, 0.f), .scale = glm::vec3(1.5f)});
		scene_ecs.AddComponent(default_entity, MeshComponent{ .mesh_name = "default" }, resource_manager);

		Entity camera_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(camera_entity, TransformComponent{.position = glm::vec3(0.f, 1.f, 0.f)});
		scene_ecs.AddComponent(camera_entity, CameraComponent{ .fov = 90.f });
		scene_ecs.AddComponent(camera_entity, PrimaryCameraComponent{});

		Entity light_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(light_entity, TransformComponent{.position = glm::vec3(5.f, 5.f, 5.f)});
		scene_ecs.AddComponent(light_entity, LightComponent{}); 

		Entity pokeball_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(pokeball_entity, TransformComponent{ .position = glm::vec3(0.f, 1.f, -7.f) });
		scene_ecs.AddComponent(pokeball_entity, MeshComponent{.mesh_name = "pokeball"}, resource_manager);

		Entity floor_entity = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(floor_entity, TransformComponent{.scale = glm::vec3(5.f)});
		scene_ecs.AddComponent(floor_entity, SurfaceComponent{ .surface_name = "floor" }, resource_manager);

		//Entity surface_entity = scene_ecs.CreateEntity();
		//scene_ecs.AddComponent(surface_entity, TransformComponent{ .position = glm::vec3(0.f, 1.f, 0.f), .scale = glm::vec3(5)});
		//scene_ecs.AddComponent(surface_entity, SurfaceComponent{ .surface_name = "test_surface" }, resource_manager);

	}

	Entity screen_entity = scene_ecs.CreateEntity();
	scene_ecs.AddComponent(screen_entity, ScreenComponent{ (int)program.windows->program_window->width, (int)program.windows->program_window->height, "fxaa" });

	screen_info_entity = scene_ecs.CreateEntity();
	scene_ecs.AddComponent(screen_info_entity, ScreenInfoComponent{
		(int)program.windows->program_window->width,
		(int)program.windows->program_window->height });

	std::cout << "scene generation completed" << std::endl;
}

void Scene::OnResize(int width, int height)
{
	auto& info = scene_ecs.GetComponent<ScreenInfoComponent>(screen_info_entity);
	info.width = width;
	info.height = height;
}

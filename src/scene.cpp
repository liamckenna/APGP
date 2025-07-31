#include "scene.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#include "program.h"
#include "ecs_manager.h"
#include "light_system.h"
#include "render_system.h"
#include "input_system.h"
#include "window.h"
#include "util.h"
#include <iostream>
#include <cctype>


Scene::Scene(Program& program) : scene_ecs(), resource_manager(), program(program) {

	
	name = "My Scene";
	scene_ecs.AddSystem<RenderSystem>(resource_manager, program.shader_manager);
	scene_ecs.AddSystem<LightSystem>(program.shader_manager);
	scene_ecs.AddSystem<InputSystem>(program.input_manager);

	Entity screen_entity = scene_ecs.CreateEntity();
	scene_ecs.AddComponent(screen_entity, ScreenComponent{});
	
	Entity camera_entity = scene_ecs.CreateEntity();
	TransformComponent camera_transform = TransformComponent{ .position = glm::vec3(8.f, 7.f, 8.f) };
	glm::vec3 camera_direction = glm::normalize(glm::vec3(-0.5f, 0, -0.5f));
	camera_transform.SetDirection(camera_direction);
	camera_transform.RotatePitch(-45.f, 1.f);
	scene_ecs.AddComponent(camera_entity, camera_transform);
	scene_ecs.AddComponent(camera_entity, CameraComponent{ .fov = 90.f, .aspect_ratio = window_width/float(window_height) });
	scene_ecs.AddComponent(camera_entity, PrimaryCameraComponent{});
	
	Entity sunlight_entity = scene_ecs.CreateEntity();
	TransformComponent sun_transform = TransformComponent{ .position = glm::vec3(0, 10, 0) };
	glm::vec3 sunlight_direction = glm::normalize(glm::vec3(0, -1, 0));
	sun_transform.SetDirection(sunlight_direction);
	scene_ecs.AddComponent(sunlight_entity, sun_transform);
	scene_ecs.AddComponent(sunlight_entity, LightComponent{});
	scene_ecs.AddComponent(sunlight_entity, DirectionalLightComponent{});
	
	Entity surface_entity = scene_ecs.CreateEntity();
	scene_ecs.AddComponent(surface_entity, TransformComponent{ .position = glm::vec3(6.f, 3.5f, 6.f), .scale = glm::vec3(20) });
	scene_ecs.AddComponent(surface_entity, SurfaceComponent{ .surface_name = "test_surface" }, resource_manager);

	Entity other_floor_entity = scene_ecs.CreateEntity();
	scene_ecs.AddComponent(other_floor_entity, TransformComponent{ .position = glm::vec3(0.f, 3.f, 0.f), .scale = glm::vec3(0.1f) });
	scene_ecs.AddComponent(other_floor_entity, SurfaceComponent{ .surface_name = "other_floor" }, resource_manager);

	Entity floor_entity = scene_ecs.CreateEntity();
	scene_ecs.AddComponent(floor_entity, TransformComponent{ .position = glm::vec3(0.f, 0.f, 0.f), .scale = glm::vec3(0.5f) });
	scene_ecs.AddComponent(floor_entity, SurfaceComponent{ .surface_name = "floor" }, resource_manager);

	

	//if you wanted to stress test
	for (int i = 0; i < 10; i++) {
		std::string name = "surface_" + std::to_string(i);
		Entity e = scene_ecs.CreateEntity();
		scene_ecs.AddComponent(e, TransformComponent{ .position = glm::vec3(RandomInt(-8, 8), RandomInt(0, 9), RandomInt(-8, 8)), .scale = glm::vec3(20)});
		scene_ecs.AddComponent(e, SurfaceComponent{ .surface_name = "test_surface" }, resource_manager);
	}
	
	std::cout << "scene generation completed" << std::endl;
}
#pragma once
#include <iostream>
#include <optional>

#include "ecs_manager.h"
#include "clock.h"
#include "graphics_config.h"
#include "user.h"

struct Windows;
struct Scene;
struct Shaders;

struct Program {
	
	Windows* windows;
	Scene* scene;
	Shaders* shaders;
	
	
	std::optional<User> user;
	std::optional<GraphicsConfig> graphics_config;
	Clock clock;

	ECSManager core_ecs;

	bool hardcoded;

	Program(const std::string& filepath);



	void Run();
	void Cleanup();
};
#pragma once
#include <iostream>
#include <optional>
#include "ecs_manager.h"
#include "shader_manager.h"
#include "input_manager.h"
#include "clock.h"
#include "graphics_config.h"

struct Windows;
struct Scene;

struct Program {
	Windows* windows;
	Scene* scene;
	
	std::optional<GraphicsConfig> graphics_config;
	Clock clock;

	ECSManager core_ecs;
	ShaderManager shader_manager;
	InputManager input_manager;

	bool hardcoded;
	bool limit_fps;
	int fps_cap;
	double target_frame_time;

	Program(const std::string& filepath);

	void Run();
	void Cleanup();
};
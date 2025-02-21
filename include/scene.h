#pragma once
#include <vector>
#include <string>
#include <optional>
#include "ecs_manager.h"
#include "resource_manager.h"
struct Program;

struct Scene {

	std::string name;

	Program& program;

	ECSManager scene_ecs;

	ResourceManager resource_manager;
	
	Scene(const std::string& filepath, Program& program);
};
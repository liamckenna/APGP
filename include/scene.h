#pragma once
#include <vector>
#include <string>
#include <optional>
#include "user.h"
#include "ecs_manager.h"
#include "resource_manager.h"
struct Mesh;
struct Meshes;
struct Object;
struct Objects;
struct Material;
struct Materials;
struct FlattenedMaterial;
struct Texture;
struct Textures;
struct Camera;
struct Cameras;
struct Light;
struct Lights;
struct FlattenedLight;
struct Program;
struct Scene {

	Program* program;

	std::string name;

	std::optional<User> user;

	Cameras* cameras;

	Lights* lights;
	
	Meshes* meshes;

	Objects* objects;

	Materials* materials;

	Textures* textures;

	ECSManager scene_ecs;

	ResourceManager resource_manager;
	
	Scene(const std::string& filepath, Program* program);

	void PrintObjectTrees();

	void UpdateObjectTrees(bool rendering = false);

	void DrawObjectTrees();

	void Render();

};
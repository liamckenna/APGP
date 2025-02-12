#pragma once
#include <vector>
#include <string>
#include "shaders.h"
#include "buffers.h"
#include "user.h"

struct Mesh;
struct Object;
struct Material;
struct FlattenedMaterial;
struct Texture;
struct Camera;
struct Light;
struct FlattenedLight;
struct Program;
struct Scene {

	Program* program;

	std::string name;

	Camera* default_camera;

	Material* default_material;

	Light* default_light;

	User* user;
	
	Buffers* buffers;

	Shaders* shaders;

	std::vector<Mesh*> meshes;

	std::vector<Object*> objects;

	std::vector<Material*> materials;

	std::vector<Texture*> textures;
	
	Material* current_material;

	std::vector<Camera*> cameras;
	Camera* main_camera;

	std::vector<Light*> lights;
	Light* main_light;

	float ambient_intensity;
	
	GLuint default_draw_mode;

	Object* held_object;

	int shading_mode;
	
	Scene(const std::string& filepath, Program* program);

	void InitializeDefaultComponents();

	void InitializeDefaultCamera();

	void InitializeDefaultLighting();

	void InitializeDefaultMaterial();

	std::vector<FlattenedLight> flattenLights();

	std::vector<FlattenedMaterial> flattenMaterials();

	Mesh* GetMeshByName(std::string name);

	Camera* GetCameraByName(std::string name);

	Object* GetObjectByName(std::string name);

	Light* GetLightByName(std::string name);

	void PrintObjectTrees();

	void UpdateObjectTrees(bool rendering = false);

	void UpdateLights();

	void DrawObjectTrees();

	void SetHeldObject(Object* object);

	void DropObject();
};
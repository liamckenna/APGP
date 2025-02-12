#include "scene.h"
#include "mesh.h"
#include "camera.h"
#include "light.h"
#include "buffers.h"
#include "shaders.h"
#include "material.h"
#include "texture.h"
#include "object.h"
#include <iostream>
#include <cctype>
#include "program.h"

Scene::Scene(const std::string& filepath, Program* program) {

	this->program = program;
	user = program->user;

	nlohmann::json data = ReadJsonFromFile(filepath);

	name = data["name"];

	if (data.contains("default_draw_mode")) SetDefaultDrawMode(data["default_draw_mode"]);
	else SetDefaultDrawMode("GL_TRIANGLES");
	if (data.contains("shading_mode")) shading_mode = data["shading_mode"];
	else shading_mode = 3;
	
	InitializeDefaultComponents();


	//for (int i = 0; i < data["cameras"].size(); i++) {
	//	cameras.push_back(new Camera());
	//}
	//CameraGeneration(scene, data);


	//LightGeneration(scene, data);
	//MeshGeneration(scene, data);
	//ObjectGeneration(scene, data);
	//SetHeldObject(GetObjectByName("camera shell"));

	buffers = new Buffers();


	std::cout << "scene generation completed" << std::endl;


}

std::vector<FlattenedLight> Scene::flattenLights() {
	std::vector<FlattenedLight> flatLights;

	for (const auto& light : this->lights) {
		FlattenedLight flatLight;
		flatLight.position = light->t->global.pos;
		flatLight.strength = light->strength;
		flatLight.color = light->c.ToVec3();
		flatLight.active = (light->active_local && light->active_global);
		flatLights.push_back(flatLight);
	}
	return flatLights;
}

std::vector<FlattenedMaterial> Scene::flattenMaterials() {
	std::vector<FlattenedMaterial> flat_materials;

	for (const auto& material : this->materials) {
		FlattenedMaterial flat_material;
		flat_material.index		= material->index;
		flat_material.tris		= material->tris;
		flat_material.edges		= material->edges;
		flat_material.refractive_index = material->refractive_index;
		flat_material.dif_color = material->colors.dif;
		flat_material.amb_color = material->colors.amb;
		flat_material.spc_color = material->colors.spc;
		flat_material.ems_color = material->colors.ems;
		flat_material.shininess = material->shininess;
		flat_material.roughness = material->roughness;
		flat_material.opacity 	= material->opacity;
		flat_material.metallic	= material->metallic;
		flat_material.dif_texture_index = (material->textures.dif == nullptr) ? -1 : material->textures.dif->index;
		flat_material.nrm_texture_index = (material->textures.nrm == nullptr) ? -1 : material->textures.nrm->index;
		flat_material.bmp_texture_index = (material->textures.bmp == nullptr) ? -1 : material->textures.bmp->index;
		flat_material.spc_texture_index = (material->textures.spc == nullptr) ? -1 : material->textures.spc->index;
		flat_material.rgh_texture_index = (material->textures.rgh == nullptr) ? -1 : material->textures.rgh->index;
		flat_material.dsp_texture_index = (material->textures.dsp == nullptr) ? -1 : material->textures.dsp->index;
		flat_material.aoc_texture_index = (material->textures.aoc == nullptr) ? -1 : material->textures.aoc->index;
		flat_material.opc_texture_index = (material->textures.opc == nullptr) ? -1 : material->textures.opc->index;
		flat_material.ems_texture_index = (material->textures.ems == nullptr) ? -1 : material->textures.ems->index;
		flat_material.hgt_texture_index = (material->textures.hgt == nullptr) ? -1 : material->textures.hgt->index;
		flat_material.met_texture_index = (material->textures.met == nullptr) ? -1 : material->textures.met->index;

		flat_materials.push_back(flat_material);
	}
	return flat_materials;
}

Mesh* Scene::GetMeshByName(std::string name) {
	for (auto& mesh : meshes) {
		if (mesh->name == name) return mesh;
	}
	return nullptr;
}

Camera* Scene::GetCameraByName(std::string name) {
	for (auto& camera : cameras) {
		if (camera->name == name) return camera;
	}
	return nullptr;
}

Object* Scene::GetObjectByName(std::string name) {
	for (auto& object : objects) {
		if (object->name == name) return object;
	}
	return nullptr;
}

Light* Scene::GetLightByName(std::string name) {
	for (auto& light : lights) {
		if (light->name == name) return light;
	}
	return nullptr;
}

void Scene::UpdateLights() {
	for (auto& light : lights) {
		light->t->UpdateGlobal();
	}
}

void Scene::PrintObjectTrees() {
	std::cout << "BEGINNING TREE" << std::endl;
	for (auto& object : objects) {
		if (object->parent == nullptr) object->PrintTree(0);
	}
	std::cout << "TREE HAS CONCLUDED" << std::endl;
}

void Scene::DrawObjectTrees() {
	for (auto& object : objects) {
		object->DrawTree(main_camera);
	}
}

void Scene::UpdateObjectTrees(bool rendering) {
	for (auto& object : objects) {
		object->UpdateTree(rendering);
	}
}

void Scene::SetHeldObject(Object* object) {
	if (object != nullptr) held_object = object;
}

void Scene::DropObject() {
	held_object = nullptr;
}

void Scene::InitializeDefaultComponents() {

	InitializeDefaultCamera();
	InitializeDefaultMaterial();
	InitializeDefaultLighting();
}

void Scene::InitializeDefaultCamera() {

	default_camera = new Camera();
	default_camera->t->global.Translate(0.f, 0.f, 2.f, 1.f);
	default_camera->LookAt(glm::vec3(0.f, 0.f, 0.f));
	cameras.push_back(default_camera);

}

void Scene::InitializeDefaultMaterial() {
	default_material = new Material();
	current_material = default_material;
	materials.push_back(default_material);
}

void Scene::InitializeDefaultLighting() {
	default_light = new Light();
	lights.push_back(default_light);
}
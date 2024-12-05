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
Scene::Scene() {

};

Scene::Scene(std::string file_name) {

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
		flat_material.dif_color = material->colors.dif;
		flat_material.amb_color = material->colors.amb;
		flat_material.spc_color = material->colors.spc;
		flat_material.ems_color = material->colors.ems;
		flat_material.shininess = material->shininess;
		flat_material.glossiness= material->glossiness;
		flat_material.opacity 	= material->opacity;
		flat_material.dif_texture_index = (material->textures.dif == nullptr) ? -1 : material->textures.dif->index;
		flat_material.nrm_texture_index = (material->textures.nrm == nullptr) ? -1 : material->textures.nrm->index;
		flat_material.bmp_texture_index = (material->textures.bmp == nullptr) ? -1 : material->textures.bmp->index;
		flat_material.spc_texture_index = (material->textures.spc == nullptr) ? -1 : material->textures.spc->index;
		flat_material.gls_texture_index = (material->textures.gls == nullptr) ? -1 : material->textures.gls->index;
		flat_material.dsp_texture_index = (material->textures.dsp == nullptr) ? -1 : material->textures.dsp->index;
		flat_material.aoc_texture_index = (material->textures.aoc == nullptr) ? -1 : material->textures.aoc->index;
		flat_material.opc_texture_index = (material->textures.opc == nullptr) ? -1 : material->textures.opc->index;
		flat_material.ems_texture_index = (material->textures.ems == nullptr) ? -1 : material->textures.ems->index;
		flat_material.hgt_texture_index = (material->textures.hgt == nullptr) ? -1 : material->textures.hgt->index;
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

void Scene::SetDefaultDrawMode(const std::string& mode) {
	if (mode == "GL_POINTS") default_draw_mode = GL_POINTS;
	else if (mode == "GL_LINES") default_draw_mode = GL_LINES;
	else if (mode == "GL_LINE_STRIP") default_draw_mode = GL_LINE_STRIP;
	else if (mode == "GL_LINE_LOOP") default_draw_mode = GL_LINE_LOOP;
	else if (mode == "GL_TRIANGLES") default_draw_mode = GL_TRIANGLES;
	else if (mode == "GL_TRIANGLE_STRIP") default_draw_mode = GL_TRIANGLE_STRIP;
	else if (mode == "GL_TRIANGLE_FAN") default_draw_mode = GL_TRIANGLE_FAN;
	else default_draw_mode = GL_TRIANGLES;
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
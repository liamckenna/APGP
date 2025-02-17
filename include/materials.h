#pragma once
#include "material.h"
#include <vector>
#include "util.h"
struct Materials {

	std::vector<Material*> materials;
	Scene* scene;
	Material* dval;
	Material* current;
	int it;

	GLuint ubo;
	GLuint binding_point;

	Materials(Scene* scene);
	std::vector<FlattenedMaterial> Flatten();
	int IterateIndex();
	void InsertDefault();
	void Insert(const nlohmann::json& data);
	void Insert(Material* material);

	Material* GetByName(std::string name);

	void SetupBuffer();
	void GenerateBuffer();
	void PopulateBuffer(std::vector<FlattenedMaterial> flattened_materials);
	void BindBuffer();
};
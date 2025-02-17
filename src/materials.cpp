#include "materials.h"
#include "util.h"
struct Scene;

Materials::Materials(Scene* scene) {
	it = 0;
	this->scene = scene;
	InsertDefault();
}


void Materials::SetupBuffer() {

	GenerateBuffer();
	PopulateBuffer(Flatten());
	BindBuffer();

}

void Materials::GenerateBuffer() {
	glGenBuffers(1, &ubo);
	binding_point = MATERIAL_BINDING_POINT;
}

void Materials::PopulateBuffer(std::vector<FlattenedMaterial> flattened_materials) {

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(FlattenedMaterial) * materials.size(), flattened_materials.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void Materials::BindBuffer() {

	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);

}

void Materials::InsertDefault() {
	dval = new Material();
	current = dval;
	materials.push_back(dval);
	dval->idx = IterateIndex();
	dval->scene = scene;
}

void Materials::Insert(const nlohmann::json& data) {

	Material* material = new Material(data, scene);
	materials.push_back(material);
	material->idx = IterateIndex();
}

void Materials::Insert(Material* material) {
	materials.push_back(material);
	material->idx = IterateIndex();
}

int Materials::IterateIndex() {
	int idx = it;
	it++;
	return idx;
}

Material* Materials::GetByName(std::string name) {

	for (auto& material : materials) {
		if (material->name == name) return material;
	}
	return nullptr;

}

std::vector<FlattenedMaterial> Materials::Flatten() {
	std::vector<FlattenedMaterial> flattened_materials;

	for (const auto& material : this->materials) {
		FlattenedMaterial flattened_material;
		flattened_material.index = material->idx;
		flattened_material.tris = material->tris;
		flattened_material.edges = material->edges;
		flattened_material.refractive_index = material->refractive_index;
		flattened_material.dif_color = material->colors.dif;
		flattened_material.amb_color = material->colors.amb;
		flattened_material.spc_color = material->colors.spc;
		flattened_material.ems_color = material->colors.ems;
		flattened_material.shininess = material->shininess;
		flattened_material.roughness = material->roughness;
		flattened_material.opacity = material->opacity;
		flattened_material.metallic = material->metallic;
		flattened_material.dif_texture_index = (material->textures.dif == nullptr) ? -1 : material->textures.dif->index;
		flattened_material.nrm_texture_index = (material->textures.nrm == nullptr) ? -1 : material->textures.nrm->index;
		flattened_material.bmp_texture_index = (material->textures.bmp == nullptr) ? -1 : material->textures.bmp->index;
		flattened_material.spc_texture_index = (material->textures.spc == nullptr) ? -1 : material->textures.spc->index;
		flattened_material.rgh_texture_index = (material->textures.rgh == nullptr) ? -1 : material->textures.rgh->index;
		flattened_material.dsp_texture_index = (material->textures.dsp == nullptr) ? -1 : material->textures.dsp->index;
		flattened_material.aoc_texture_index = (material->textures.aoc == nullptr) ? -1 : material->textures.aoc->index;
		flattened_material.opc_texture_index = (material->textures.opc == nullptr) ? -1 : material->textures.opc->index;
		flattened_material.ems_texture_index = (material->textures.ems == nullptr) ? -1 : material->textures.ems->index;
		flattened_material.hgt_texture_index = (material->textures.hgt == nullptr) ? -1 : material->textures.hgt->index;
		flattened_material.met_texture_index = (material->textures.met == nullptr) ? -1 : material->textures.met->index;

		flattened_materials.push_back(flattened_material);
	}
	return flattened_materials;
}
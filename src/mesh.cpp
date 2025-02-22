#include "mesh.h"
#include <iostream>
#include "scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#undef GLM_ENABLE_EXPERIMENTAL

void Mesh::SetupBuffers() {
	GenerateBuffers();
	PopulateBuffers(Flatten());
}

void Mesh::GenerateBuffers() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &ssbo);
}

void Mesh::PopulateBuffers(std::vector<FlatVertex> flat_vertices) {

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, flat_vertices.size() * sizeof(FlatVertex), flat_vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FlatVertex), (void*)offsetof(FlatVertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FlatVertex), (void*)offsetof(FlatVertex, texcoord));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FlatVertex), (void*)offsetof(FlatVertex, normal));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, material_index.size() * sizeof(int), material_index.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_BINDING_POINT, ssbo);
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(int), (void*)0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindVertexArray(0);
}

std::vector<FlatVertex> Mesh::Flatten() {
	std::vector<FlatVertex> flat_vertices;
	flat_vertices.reserve(vertices.size());

	for (const Vertex& v : vertices) {
		FlatVertex fv;
		fv.position = v.position;
		fv.texcoord = v.texcoord;
		fv.normal =	v.normal;
		flat_vertices.push_back(fv);
	}

	return flat_vertices;
}
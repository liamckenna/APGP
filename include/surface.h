#pragma once
#include <vector>
#include <GL/glew.h>
#include "uniforms.h"
#include "vertex.h"
#include "flat_vertex.h"
#include "vertex_key.h"

struct Surface {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<int> material_index;
	std::unordered_map<VertexKey, int> unique_vertices;
	std::vector<glm::ivec4> patches;
	int patch_size = 16;
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint ssbo = 0;
	std::string name;

	Surface() = default;

	void SetupBuffers();
	void GenerateBuffers();
	void PopulateBuffers(std::vector<FlatVertex> flat_vertices);
	std::vector<FlatVertex> Flatten();
	void CreatePatches();

	float texel_size;

	float tess_level;

	bool selected;

	// for iPass
	GLuint patch_tess_level_texture;
	GLuint connectivity_texture;

	int num_patches;
	glm::vec3 bbox[2];

};
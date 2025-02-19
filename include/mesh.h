#pragma once
#include <vector>
#include <GL/glew.h>
#include "uniforms.h"
#include "vertex.h"
#include "flat_vertex.h"
#include "vertex_key.h"

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<int> material_index;
	std::unordered_map<VertexKey, int> unique_vertices;
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint ssbo = 0;
	std::string name;

	Mesh() = default;

	void SetupBuffers();
	void GenerateBuffers();
	void PopulateBuffers(std::vector<FlatVertex> flat_vertices);
	std::vector<FlatVertex> Flatten();
};
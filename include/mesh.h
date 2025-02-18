#pragma once
#include <vector>
#include <GL/glew.h>
#include "camera.h"
#include "uniforms.h"
#include "vertex.h"
#include "vertex_key.h"


struct Mesh {

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<int> face_material;
	std::unordered_map<VertexKey, int> unique_vertices;
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	std::string name;

	Mesh() = default;

	void SetupBuffers();
	void GenerateBuffers();
	void PopulateBuffers(std::vector<FlattenedVertex> flattened_vertices);
	std::vector<FlattenedVertex> Flatten();
	
};
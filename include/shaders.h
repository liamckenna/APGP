#pragma once
#include <vector>
#include <string>
#include "shader.h"
#include "GL/glew.h"
struct Shaders{
	
	std::vector<Shader> vertex_shaders;
	std::vector<Shader> geometry_shaders;
	std::vector<Shader> fragment_shaders;

	GLuint shader_program;
	GLuint composite_program;

	void CleanupShaders();
};
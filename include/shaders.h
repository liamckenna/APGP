#pragma once
#include <unordered_map>
#include <string>
#include "json.h"
#include "GL/glew.h"
#include "uniforms.h"

struct Program;

struct Shaders{
	
	GLuint gl_program;

	std::unordered_map<std::string, std::vector<GLuint>> compiledShaders;
	std::unordered_map<std::string, int> activeShaders;

	Uniforms uniforms;

	Program* program;

	Shaders(const std::string& filepath, Program* program);
	~Shaders();

	bool Initialize(const nlohmann::json& data);
	GLuint CompileAndAttachShader(const std::string& filePath, GLenum type, GLuint program);
	std::string LoadShader(const std::string& filepath);
	void SetActiveShader(const std::string& shaderType, int newIndex);

	void SetUniformValues(const nlohmann::json& data);
	
};
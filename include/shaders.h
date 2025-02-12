#pragma once
#include <unordered_map>
#include <string>
#include "json.h"
#include "GL/glew.h"

struct Shaders{
	
	GLuint main_program;

	std::unordered_map<std::string, std::vector<GLuint>> compiledShaders;
	std::unordered_map<std::string, int> activeShaders;

	std::unordered_map<std::string, GLint> uniform_locations;

	Program* program;

	Shaders(const std::string& filepath, Program* program);
	~Shaders();

	bool Initialize(const nlohmann::json& data);
	GLuint CompileAndAttachShader(const std::string& filePath, GLenum type, GLuint program);
	std::string LoadShader(const std::string& filepath);
	void SetActiveShader(const std::string& shaderType, int newIndex);

	void CacheUniformLocations(); // New function to dynamically detect uniforms
	GLint GetUniformLocation(const std::string& name); // Lookup function
	
};
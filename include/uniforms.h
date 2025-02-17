#pragma once
#include "uniform.h"
#include <vector>
#include <unordered_map>


using DataType = std::variant<int, unsigned int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;
struct Shaders;

struct Uniforms {

	std::unordered_map < std::string, Uniform> uniforms;

	Shaders* shaders;

	Uniforms();
	Uniforms(Shaders* shaders);

	Uniform& GetByName(std::string name);

	void FindAndUpdate(std::string name, DataType value);

	void CacheUniforms();
};
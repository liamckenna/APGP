#pragma once

#include <iostream>
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <variant>
#include <typeindex>
#include "util.h"


using DataType = std::variant<int, unsigned int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;

struct Uniform {

	std::string name;
	GLint location;
	GLenum type;
	DataType value;

	Uniform() = default;

	Uniform(std::string name, GLint location, GLenum type, DataType value) {
		this->name = name;
		this->location = location;
		this->type = type;
		this->value = value;
	};
	Uniform(std::string name, GLint location, GLenum type) {
		this->name = name;
		this->location = location;
		this->type = type;
		//SetDefaultValue(type);
	};

	void Update(DataType new_value) {
		if (location == -1) return;
		if (value == new_value) return;
		else value = new_value;
		switch (type) {
		case GL_INT:
		case GL_SAMPLER_2D:
			glUniform1i(location, std::get<int>(value));
			break;
		case GL_UNSIGNED_INT:
			glUniform1i(location, std::get<unsigned int>(value));
			break;
		case GL_FLOAT:
			glUniform1f(location, std::get<float>(value));
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(location, 1, glm::value_ptr(std::get<glm::vec2>(value)));
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(location, 1, glm::value_ptr(std::get<glm::vec3>(value)));
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(location, 1, glm::value_ptr(std::get<glm::vec4>(value)));
			break;
		case GL_FLOAT_MAT2:
			glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat2>(value)));
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat3>(value)));
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat4>(value)));
			break;
		default:
			std::cerr << "Error: Unsupported uniform data type." << std::endl;
			break;
		}
	};

	void SetDefaultValue(GLenum type) {
		switch (type) {
			case GL_INT:           value = 0;
				break;
			case GL_UNSIGNED_INT:  value = 0u;
				break;
			case GL_FLOAT:         value = 0.0f;
				break;
			case GL_FLOAT_VEC2:    value = glm::vec2(0.0f);
				break;
			case GL_FLOAT_VEC3:    value = glm::vec3(0.0f);
				break;
			case GL_FLOAT_VEC4:    value = glm::vec4(0.0f);
				break;
			case GL_FLOAT_MAT2:    value = glm::mat2(1.0f);  // Identity matrix
				break;
			case GL_FLOAT_MAT3:    value = glm::mat3(1.0f);  // Identity matrix
				break;
			case GL_FLOAT_MAT4:    value = glm::mat4(1.0f);  // Identity matrix
				break;
			default:
				std::cerr << "UNIFORM NAME: " << name << std::endl;
				std::cerr << "Error: Unsupported uniform type for default value." << std::endl;
				value = 0;
				break;
		}
	}
};


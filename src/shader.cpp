#include "shader.h"

Shader::Shader(std::string file_name, GLuint data, byte type) {
	this->file_name = file_name;
	this->data = data;
	this->type = type;
};
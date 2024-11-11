#pragma once
#include <string>
#include "GL/glew.h"
#include "byte.h"
enum SHADER_TYPE {
	VERTEX = 0x00,
	GEOMETRY = 0X01,
	FRAGMENT = 0X10
};

struct Shader{
	std::string file_name;
	GLuint data;
	byte type;

	Shader(std::string file_name, GLuint data, byte type);
};
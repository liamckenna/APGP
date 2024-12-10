#pragma once
#include <string>
#include "GL/glew.h"
#include "byte.h"
enum SHADER_TYPE {
	VERTEX = 0x01,
	GEOMETRY = 0x02,
	FRAGMENT = 0x03,
	COMPOSITE_VERTEX = 0X04,
	COMPOSITE_FRAGMENT = 0x05
};

struct Shader{
	std::string file_name;
	GLuint data;
	byte type;

	Shader(std::string file_name, GLuint data, byte type);
};
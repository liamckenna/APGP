#pragma once
#include <string>
#include <GL/glew.h>
enum TEXTURE_TYPES {
	TXT_DIFFUSE 			= 0x0000,
	TXT_ALBEDO 				= 0X0000,
	TXT_NORMAL 				= 0X0001,
	TXT_BUMP 				= 0X0002,
	TXT_SPECULAR			= 0X0003,
	TXT_ROUGHNESS			= 0X0004,
	TXT_GLOSSINESS			= 0X0004,
	TXT_DISPLACEMENT		= 0X0005,
	TXT_AMBIENTOCCLUSION	= 0X0006,
	TXT_OPACITY				= 0X0007,
	TXT_ALPHA 				= 0X0007,
	TXT_EMISSIVE			= 0X0008,
	TXT_HEIGHT				= 0X0009
};

struct Texture {
	GLuint id;
	TEXTURE_TYPES type;
	std::string file_path;
	GLint index;
	GLint wrap_s;
	GLint wrap_t;
	GLint min_filter;
	GLint mag_filter;

	Texture(const std::string& file_path, const TEXTURE_TYPES& type);

	bool LoadTexture();

	void Bind(GLenum texture_unit) const;
};
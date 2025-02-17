#pragma once
#include "GL/glew.h"

struct Framebuffer {
	GLuint framebuffer_vao;
	GLuint framebuffer_vbo;
	GLuint framebuffer;
	GLuint color_attachment;
	GLuint depth_attachment;
	GLuint accum_color_tex;
	GLuint accum_alpha_tex;

	Framebuffer();
	//void GenerateFramebuffers(); //todo
	//void CleanupFramebuffers(); //todo
};
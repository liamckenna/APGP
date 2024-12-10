#pragma once
#include <vector>
#include "GL/glew.h"

struct Buffers {
	std::vector<GLuint> vertex_arrays;
	std::vector<GLuint> vertex_buffers;
	std::vector<GLuint> element_buffers;
	std::vector<GLuint> color_buffers;
	std::vector<GLuint> triangle_index_buffers;
	
	GLuint light_uniform_buffer;
	GLuint light_binding_point;

	GLuint shader_uniform_buffer;
	GLuint shader_binding_point;

	GLuint texture_uniform_buffer;
	GLuint texture_binding_point;

	GLuint framebuffer_vao;
	GLuint framebuffer_vbo;
	GLuint framebuffer;
	GLuint color_attachment;
	GLuint depth_attachment;
	GLuint accum_color_tex;
	GLuint accum_alpha_tex;


	Buffers();
	void GenerateBuffers(); //todo
	void CleanupBuffers(); //todo
};
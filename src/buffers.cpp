#include "buffers.h"

Buffers::Buffers() {
	light_binding_point = 0;
	shader_binding_point = 1;
	texture_binding_point = 2;
}

void Buffers::GenerateBuffers() {
	GLuint va, vb, eb, cb, tib, lub;
	vertex_arrays.push_back(va);
	vertex_buffers.push_back(vb);
	element_buffers.push_back(eb);
	color_buffers.push_back(cb);
	triangle_index_buffers.push_back(tib);
}

void Buffers::CleanupBuffers() {
	for (auto& vertex_array : vertex_arrays) {
		glDeleteVertexArrays(1, &vertex_array);
	}
	for (auto& vertex_buffer : vertex_buffers) {
		glDeleteBuffers(1, &vertex_buffer);
	}
	for (auto& element_buffer : element_buffers) {
		glDeleteBuffers(1, &element_buffer);
	}
	for (auto& color_buffer : color_buffers) {
		glDeleteBuffers(1, &color_buffer);
	}
	for (auto& triangle_index_buffer : triangle_index_buffers) {
		glDeleteBuffers(1, &triangle_index_buffer);
	}
}
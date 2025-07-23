#include "surface_renderer.h"
#include "patch.h"
#include "surface.h"
#include <numeric>
#include <fstream>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
using std::vector;

void SurfaceRenderer::RenderSurface(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix,
									glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, GLuint vao, GLuint vbo, GLuint ebo,
									GLuint patch_buffer, GLuint patch_shadow_buffer, GLuint launch_point_buffer,
									uint vertex_count, int surface_id, ShaderManager& shader_manager) {

	shader_manager.UseShader("render_pass");

	glm::mat4 model_view_matrix = view_matrix * model_matrix;
	shader_manager.SetUniform("ModelViewMatrix", model_view_matrix);
	shader_manager.SetUniform("ProjectionMatrix", projection_matrix);
	shader_manager.SetUniform("NormalMatrix", glm::inverseTranspose(glm::mat3(model_view_matrix)));

	shader_manager.SetUniform("Ka", Ka);
	shader_manager.SetUniform("Kd", Kd);
	shader_manager.SetUniform("Ks", Ks);

	shader_manager.SetUniform("surface_id", surface_id);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, patch_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_shadow_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, patch_shadow_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, launch_point_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, launch_point_buffer);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glDrawElements(GL_PATCHES, vertex_count, GL_UNSIGNED_INT, 0);
}

void SurfaceRenderer::UpdatePatchTessLevels(GLuint vertex_buffer, GLuint patch_buffer, glm::mat4 MVP,
											int vertex_count, int surface_id, float pixel_size, ShaderManager& shader_manager) {

	shader_manager.UseShader("patch_tess_pass");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, patch_buffer);

	shader_manager.SetUniform("num_vertices", vertex_count);
	shader_manager.SetUniform("pixel_size", pixel_size);
	shader_manager.SetUniform("MVP", MVP);
	shader_manager.SetUniform("light_pass", 0);

	int groupSize = 32;
	shader_manager.SetUniform("surface_id", surface_id);

	glDispatchCompute(vertex_count / groupSize + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

}

void SurfaceRenderer::WriteDepthBuffer(GLuint vertex_buffer, GLuint launch_point_buffer, GLuint depth_buffer_texture,
									GLuint patch_depth_buffer, GLuint patch_span_buffer, GLuint light_mvp_buffer,
									int vertex_count, int surface_id, float pixel_size, ShaderManager& shader_manager) {
	
	shader_manager.UseShader("patch_tess_pass");

	uint patch_count = (vertex_count + 15) / 16;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_depth_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, patch_depth_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_span_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, patch_span_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_mvp_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, light_mvp_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, launch_point_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, launch_point_buffer);

	GLuint debug_buffer;
	glGenBuffers(1, &debug_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, debug_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * patch_count, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, debug_buffer);

	shader_manager.SetUniform("num_vertices", vertex_count);
	shader_manager.SetUniform("pixel_size", pixel_size);
	shader_manager.SetUniform("light_pass", 1);
	shader_manager.SetUniform("surface_id", surface_id);

	int groupSize = 32;

	glDispatchCompute(vertex_count / groupSize + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

	shader_manager.UseShader("patch_depth_buffer");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_depth_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, patch_depth_buffer);

	std::vector<glm::uint> launch_point_values(surface_id + 1);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, launch_point_buffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::uint) * (surface_id + 1), launch_point_values.data());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, launch_point_buffer);
	uint launch_point = launch_point_values[surface_id];

	std::vector<glm::uvec4> patch_span_values(launch_point + patch_count);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_span_buffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::uvec4) * (launch_point + patch_count), patch_span_values.data());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, patch_span_buffer);

	glBindImageTexture(0, depth_buffer_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	shader_manager.SetUniform("access_mode", 0); //0 = WRITE, 1 = READ

	for (uint i = 0; i < patch_count; i++) {
		glm::uvec4 span = patch_span_values[launch_point + i];
		uint min_x = span.x, max_x = span.y;
		uint min_y = span.z, max_y = span.w;

		uint size_x = max_x - min_x + 1;
		uint size_y = max_y - min_y + 1;

		shader_manager.SetUniform("global_patch_id", static_cast<int>(launch_point + i));

		glDispatchCompute(size_x, size_y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	}
}

void SurfaceRenderer::ReadDepthBuffer(GLuint patch_depth_buffer, GLuint patch_span_buffer, GLuint patch_shadow_buffer, 
									GLuint launch_point_buffer, GLuint depth_buffer_texture, int surface_id,
									int vertex_count, ShaderManager& shader_manager) {

	shader_manager.UseShader("patch_depth_buffer");

	uint patch_count = (vertex_count + 15) / 16;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_depth_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, patch_depth_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_span_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, patch_span_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, launch_point_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, launch_point_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_shadow_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, patch_shadow_buffer);

	GLuint debug_buffer;
	glGenBuffers(1, &debug_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, debug_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * patch_count, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, debug_buffer);

	glBindImageTexture(0, depth_buffer_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	shader_manager.SetUniform("access_mode", 1); //0 = WRITE, 1 = READ
	shader_manager.SetUniform("surface", surface_id);

	glDispatchCompute(patch_count, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

}

void SurfaceRenderer::updateIPASSTexture(Surface* surface, glm::mat4 MVP, float pixel_size)
{
	vector<float> patch_tess_levels = determine_patch_tess_levels(&surface->vertices[0], surface->num_patches, 3, 3, MVP, pixel_size);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, surface->patch_buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, patch_tess_levels.size() * sizeof(float), &patch_tess_levels[0]);
	int numPatches = 4;
	std::vector<float> data(numPatches);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(float), data.data());
}
#include "surface_renderer.h"
#include "patch.h"
#include "surface.h"
#include <numeric>
#include <fstream>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
using std::vector;

void SurfaceRenderer::WriteDepthBuffer(GLuint vertex_buffer, GLuint launch_point_buffer, GLuint depth_buffer_texture,
	 GLuint light_mvp_buffer, GLuint vao, GLuint ebo, glm::mat4 mvp,
	int vertex_count, int surface_id, float pixel_size, ShaderManager& shader_manager)
{
	//get tile indices
	shader_manager.UseShader("patch_tess_pass");

	uint patch_count = (vertex_count + 15) / 16;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer);

	GLuint index_buffer;
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, index_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * patch_count * 54, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, index_buffer);

	GLuint debug_buffer;
	glGenBuffers(1, &debug_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, debug_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * patch_count, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, debug_buffer);

	shader_manager.SetUniform("num_vertices", vertex_count);
	shader_manager.SetUniform("pixel_size", pixel_size);
	shader_manager.SetUniform("light_pass", 1);
	int groupSize = 32;

	glDispatchCompute(vertex_count / groupSize + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

	//write depth buffer
	shader_manager.UseShader("patch_shadow");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_mvp_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, light_mvp_buffer);

	glBindImageTexture(0, depth_buffer_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	
	shader_manager.SetUniform("surface_id", surface_id);
	
	std::vector<int> index_values(patch_count * 54);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, index_buffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * patch_count * 54, index_values.data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glDrawElements(GL_TRIANGLES, index_values.size(), GL_UNSIGNED_INT, 0);
	
	//visualize tiling with lines
	shader_manager.UseShader("visualize_tiles");
	
	shader_manager.SetUniform("MVP_MATRIX", mvp);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glDrawElements(GL_TRIANGLES, index_values.size(), GL_UNSIGNED_INT, 0);
}

void SurfaceRenderer::UpdatePatchTessLevels(GLuint vertex_buffer, GLuint patch_buffer, glm::mat4 MVP,
	int vertex_count, int surface_id, float pixel_size, ShaderManager& shader_manager)
{
	shader_manager.UseShader("patch_tess_pass");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, patch_buffer);

	uint patch_count = (vertex_count + 15) / 16;
	GLuint debug_buffer;
	glGenBuffers(1, &debug_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, debug_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * patch_count, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, debug_buffer);

	shader_manager.SetUniform("num_vertices", vertex_count);
	shader_manager.SetUniform("pixel_size", pixel_size);
	shader_manager.SetUniform("MVP", MVP);
	shader_manager.SetUniform("light_pass", 0);

	int groupSize = 32;
	
	glDispatchCompute(vertex_count / groupSize + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
}

void SurfaceRenderer::RenderSurface(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix,
									glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, GLuint vao, GLuint vbo, GLuint ebo,
									GLuint patch_buffer, uint vertex_count, int surface_id, ShaderManager& shader_manager,
									GLuint depth_buffer_texture, GLuint light_mvp_buffer) 
{
	shader_manager.UseShader("render_pass");

	glm::mat4 model_view_matrix = view_matrix * model_matrix;
	shader_manager.SetUniform("ModelViewMatrix", model_view_matrix);
	glm::mat4 model_view_inverse = glm::inverse(model_view_matrix);
	shader_manager.SetUniform("ModelViewInverse", model_view_inverse);

	shader_manager.SetUniform("ProjectionMatrix", projection_matrix);
	shader_manager.SetUniform("NormalMatrix", glm::inverseTranspose(glm::mat3(model_view_matrix)));

	shader_manager.SetUniform("Ka", Ka);
	shader_manager.SetUniform("Kd", Kd);
	shader_manager.SetUniform("Ks", Ks);

	shader_manager.SetUniform("surface_id", surface_id);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, patch_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_mvp_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, light_mvp_buffer);

	glBindImageTexture(0, depth_buffer_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);

	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glDrawElements(GL_PATCHES, vertex_count, GL_UNSIGNED_INT, 0);
}

void SurfaceRenderer::UpdateTessNonCompute(Surface* surface, glm::mat4 MVP, float pixel_size)
{
	vector<float> patch_tess_levels = determine_patch_tess_levels(&surface->vertices[0], surface->num_patches, 3, 3, MVP, pixel_size);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, surface->patch_buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, patch_tess_levels.size() * sizeof(float), &patch_tess_levels[0]);
	int numPatches = 4;
	std::vector<float> data(numPatches);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(float), data.data());
}

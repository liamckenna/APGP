#include "surface_renderer.h"
#include "patch.h"
#include "surface.h"
#include <numeric>
#include <fstream>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
using std::vector;



void SurfaceRenderer::renderSurface(Surface* surface, int CurrentWidth, bool is_first_frame, bool use_compute, bool light_pass,
	glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, GLuint light_patch_buffer, GLuint depth_texture,
	const glm::mat4& ModelViewMatrix, const glm::mat4& ProjectionMatrix, ShaderManager& shader_manager)
{
	float pixel_size = 2.0 / CurrentWidth; // pixel size in clipping space
	glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;
	
	if (use_compute) {
		if (light_pass) {
			iPASSLightPass(surface->vertices.size(), surface->vbo, light_patch_buffer, depth_texture, MVP, pixel_size, shader_manager);
		}
		else {
			updateIPASSTexture_CS(surface->vertices.size(), surface->vbo, surface->patch_buffer, 0, MVP, pixel_size, shader_manager);
		}
	} else
		updateIPASSTexture(surface, MVP, pixel_size);

	if (!light_pass) {
		shader_manager.UseShader("render_pass");
		shader_manager.SetUniform("ModelViewMatrix", ModelViewMatrix);
		shader_manager.SetUniform("ProjectionMatrix", ProjectionMatrix);
		shader_manager.SetUniform("NormalMatrix", glm::inverseTranspose(glm::mat3(ModelViewMatrix)));

		shader_manager.SetUniform("Ka", Ka);
		shader_manager.SetUniform("Ks", Ks);

		if (surface->selected)
			shader_manager.SetUniform("Kd", glm::vec3(0, 0, 1));
		else
			shader_manager.SetUniform("Kd", Kd);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_1D, surface->connectivity_texture);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, surface->patch_buffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, surface->patch_buffer);


		glBindVertexArray(surface->vao);
		glBindBuffer(GL_ARRAY_BUFFER, surface->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface->ebo);

		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawElements(GL_PATCHES, surface->vertices.size(), GL_UNSIGNED_INT, 0);
	}
}


void SurfaceRenderer::updateIPASSTexture(Surface* surface, const glm::mat4& MVP, float pixel_size)
{
	vector<float> patch_tess_levels = determine_patch_tess_levels(&surface->vertices[0], surface->num_patches, 3, 3, MVP, pixel_size);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, surface->patch_buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, patch_tess_levels.size() * sizeof(float), &patch_tess_levels[0]);
	int numPatches = 4;
	std::vector<float> data(numPatches);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(float), data.data());
}


void SurfaceRenderer::updateIPASSTexture_CS(int num_vertices, GLuint vbuffer_id, GLuint pbuffer_id, GLuint patch_tess_level_texture_id, const glm::mat4& MVP, float pixel_size, ShaderManager& shader_manager)
{
	shader_manager.UseShader("patch_tess_pass");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuffer_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbuffer_id);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pbuffer_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pbuffer_id);

	shader_manager.SetUniform("num_vertices", num_vertices);
	shader_manager.SetUniform("pixel_size", pixel_size);
	shader_manager.SetUniform("MVP", MVP);
	shader_manager.SetUniform("light_pass", 0);

	int groupSize = 32;
	glDispatchCompute(num_vertices / groupSize + 1, 1, 1);
	int numPatches = 4;
	std::vector<float> data(numPatches);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(float), data.data());

	//for (int i = 0; i < numPatches; i++) {
	//	std::cout << "slot " << i << ": " << data[i] << std::endl;
	//}
}

void SurfaceRenderer::iPASSLightPass(int num_vertices, GLuint vbuffer_id, GLuint pbuffer_id, GLuint depth_texture, const glm::mat4& MVP, float pixel_size, ShaderManager& shader_manager)
{
	shader_manager.UseShader("patch_tess_pass");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuffer_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbuffer_id);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pbuffer_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pbuffer_id);

	glBindImageTexture(0, depth_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);

	shader_manager.SetUniform("num_vertices", num_vertices);
	shader_manager.SetUniform("pixel_size", pixel_size);
	shader_manager.SetUniform("light_MVP", MVP);
	shader_manager.SetUniform("light_pass", 1);

	int groupSize = 32;
	glDispatchCompute(num_vertices / groupSize + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	int numPatches = 4;
	std::vector<float> data(numPatches);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(float), data.data());

	//for (int i = 0; i < numPatches; i++) {
	//	std::cout << "slot " << i << ": " << data[i] << std::endl;
	//}
}
#include "surface_renderer.h"
#include "patch.h"
#include "surface.h"
#include <numeric>
#include <fstream>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
using std::vector;



void SurfaceRenderer::renderSurface(Surface* surface, int surface_id,  int CurrentWidth, bool is_first_frame, bool use_compute, bool light_pass,
	glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, GLuint shadow_buffer, GLuint launch_point_buffer, GLuint depth_texture, glm::mat4 mvp,
	const glm::mat4& ModelViewMatrix, const glm::mat4& ProjectionMatrix, ShaderManager& shader_manager, bool debug)
{
	float pixel_size = 2.0 / CurrentWidth;

	if (use_compute) {

		updateIPASSTexture_CS(surface->vertices.size(), surface_id, surface->vbo, surface->patch_buffer, 0, mvp, pixel_size, shader_manager, debug, depth_texture);
		
	} else
		updateIPASSTexture(surface, mvp, pixel_size);

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

		shader_manager.SetUniform("surface_id", surface_id);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_1D, surface->connectivity_texture);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, surface->patch_buffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, surface->patch_buffer);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadow_buffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, shadow_buffer);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, launch_point_buffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, launch_point_buffer);

		glBindVertexArray(surface->vao);
		glBindBuffer(GL_ARRAY_BUFFER, surface->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface->ebo);
		
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawElements(GL_PATCHES, surface->vertices.size(), GL_UNSIGNED_INT, 0);
	}
	debug = false;
	if (debug) {
		std::cout << "-- Actual Render Debug --" << std::endl;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shadow_buffer);
		uint* test_values = (uint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		for (int i = 0; i < 8; ++i) {
			std::cout << i << ": " << test_values[i] << "\n";
		}
		std::cout << std::endl;
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

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


void SurfaceRenderer::updateIPASSTexture_CS(int num_vertices, int surface_id, GLuint vbuffer_id, GLuint pbuffer_id, GLuint patch_tess_level_texture_id, const glm::mat4& MVP, float pixel_size, ShaderManager& shader_manager, bool read, GLuint depth_texture)
{
	shader_manager.UseShader("patch_tess_pass");

	int patch_count = (num_vertices + 15) / 16;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuffer_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbuffer_id);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pbuffer_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pbuffer_id);

	GLuint dbug_id;

	glGenBuffers(1, &dbug_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dbug_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * patch_count, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dbug_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, dbug_id);

	glBindImageTexture(0, depth_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);


	shader_manager.SetUniform("num_vertices", num_vertices);
	shader_manager.SetUniform("pixel_size", pixel_size);
	shader_manager.SetUniform("MVP", MVP);
	shader_manager.SetUniform("light_pass", 0);

	int groupSize = 32;
	shader_manager.SetUniform("surface_id", surface_id);

	glDispatchCompute(num_vertices / groupSize + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	bool debug = false;
	//debug = true;

	if (debug) {
		std::cout << "-- Render Pass Debug --" << std::endl;
		if (num_vertices > 16) std::cout << "test_surface: " << std::endl;
		else std::cout << "floor: " << std::endl;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, dbug_id);
		glm::vec4* test_values = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		for (int i = 0; i < patch_count; ++i) {
			std::cout << "Patch " << i << ":\n";
			std::cout << "  X:    " << test_values[i].x << "\n";
			std::cout << "  Y:  " << test_values[i].y << "\n";
			std::cout << "  Z:    " << test_values[i].z << "\n";
			std::cout << "  W:    " << test_values[i].w << "\n";
		}
		std::cout << std::endl;
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}
}

void SurfaceRenderer::iPASSLightPass(int num_vertices, int surface_id, GLuint vbuffer_id, GLuint pbuffer_id, GLuint sbuffer_id, GLuint launch_point_id, GLuint depth_texture, float pixel_size, ShaderManager& shader_manager)
{
	
	shader_manager.UseShader("patch_tess_pass");

	uint patch_count = (num_vertices + 15) / 16;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuffer_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbuffer_id);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pbuffer_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pbuffer_id);

	GLuint dbug_id;
	glGenBuffers(1, &dbug_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dbug_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * patch_count, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, dbug_id);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sbuffer_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, sbuffer_id);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, launch_point_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, launch_point_id);

	GLuint dvals_id;
	glGenBuffers(1, &dvals_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dvals_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint) * patch_count, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, dvals_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLuint span_id;
	glGenBuffers(1, &span_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, span_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::uvec4) * patch_count, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, span_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindImageTexture(0, depth_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	shader_manager.SetUniform("num_vertices", num_vertices);
	shader_manager.SetUniform("pixel_size", pixel_size);
	shader_manager.SetUniform("light_pass", 1);
	shader_manager.SetUniform("surface_id", surface_id);

	int groupSize = 32;

	glDispatchCompute(num_vertices / groupSize + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	bool debug = false;
	//debug = true;

	if (debug) {
		std::cout << "-- Light Pass Debug --" << std::endl;
		if (num_vertices > 16) std::cout << "test_surface: " << std::endl;
		else std::cout << "floor: " << std::endl;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, dbug_id);
		glm::vec4* test_values = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		for (int i = 0; i < patch_count; ++i) {
			std::cout << "Patch " << i << ":\n";
			std::cout << "  X:    " << test_values[i].x << "\n";
			std::cout << "  Y:    " << test_values[i].y << "\n";
			std::cout << "  Z:    " << test_values[i].z << "\n";
			std::cout << "  W:    " << test_values[i].w << "\n";
		}
		std::cout << std::endl;
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}


}
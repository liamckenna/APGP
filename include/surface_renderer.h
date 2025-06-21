#pragma once
#include <glm/glm.hpp>
#include "shader_manager.h"
#include "surface.h"
#define ZONE_ID_RENDERGROUP 0
#define ZONE_ID_IPASS 1
#define ZONE_ID_RENDER 2
#define ZONE_ID_SCANLINE 3
#define ZONE_ID_SORT 4
#define ZONE_ID_STRIP 5

#define ZONE_ID_END 6
#define MAX_NUM_PATCH 65536 
using glm::vec4;

struct PatchBuffer {
	float TessLevel[MAX_NUM_PATCH];
};

struct SlefeBox
{
	vec4 upper;
	vec4 lower;
};

class SurfaceRenderer
{
public:

	// GL constant
	static int max_group_count_x;

	static void init_shaders();

	GLuint patch_buffer;	    // store the per patch tessellation level

private:


public:
	SurfaceRenderer()
	{
		init_gpu_resource();
	}

	~SurfaceRenderer()
	{
	}

	void init_gpu_resource()
	{

		glGenBuffers(1, &patch_buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, patch_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PatchBuffer), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}

	void renderSurface(Surface* surface, int CurrentWidth, bool is_first_frame, bool use_compute, bool light_pass,
		glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, GLuint light_patch_buffer, GLuint depth_texture,
		const glm::mat4& ModelViewMatrix, const glm::mat4& ProjectionMatrix, ShaderManager& shader_manager);

private:

	void updateIPASSTexture(Surface* surface, const glm::mat4& MVP, float pixel_size);
	void updateIPASSTexture_CS(int num_vertices, GLuint vbuffer_id, GLuint pbuffer_id, GLuint patch_tess_level_texture_id, const glm::mat4& MVP, float pixel_size, ShaderManager& shader_manager);
	void iPASSLightPass(int num_vertices, GLuint vbuffer_id, GLuint pbuffer_id, GLuint depth_texture, const glm::mat4& MVP, float pixel_size, ShaderManager& shader_manager);

};
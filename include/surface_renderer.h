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

	void WriteDepthBuffer(GLuint vertex_buffer, GLuint launch_point_buffer, GLuint depth_buffer_texture,
						GLuint light_mvp_buffer, GLuint vao, GLuint ebo, glm::mat4 mvp,
						int vertex_count, int surface_id, float pixel_size, ShaderManager& shader_manager);
	void UpdatePatchTessLevels(GLuint vertex_buffer, GLuint patch_buffer, glm::mat4 MVP,
							int vertex_count, int surface_id, float pixel_size, ShaderManager& shader_manager);
	void RenderSurface(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix,
						glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, GLuint vao, GLuint vbo, GLuint ebo,
						GLuint patch_buffer, uint vertex_count, int surface_id, ShaderManager& shader_manager, 
						GLuint depth_buffer_texture, GLuint light_mvp_buffer);
private:

	void UpdateTessNonCompute(Surface* surface, glm::mat4 MVP, float pixel_size);

};
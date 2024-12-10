#pragma once

#include "input.h"
#include "window.h"
#include <GLFW/glfw3.h>
struct User { //local user variables all packaged in one convenient place
	
	

	Window* window;
	Input* input;
	
	GLuint matrix_id;
	GLuint view_matrix_id;
	GLuint model_matrix_id;
	GLuint triangle_color_id;
	GLuint triangle_normal_id;
	GLuint camera_position_id;
	GLuint shading_mode_id;
	GLuint phong_exponent_id;
	GLuint light_count_id;
	GLuint light_position_id;
	GLuint light_strength_id;
	GLuint light_color_id;
	GLuint light_active_id;
	GLuint ambient_intensity_id;
	GLuint textures_id;

	GLuint accum_color_tex_id;
	GLuint accum_alpha_tex_id;
};
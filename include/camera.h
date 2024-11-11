#pragma once
#include "transform.h"
#include "byte.h"
#include "object.h"
#include <iostream>
#include <GL/glew.h>
#include "scene.h"
enum PROJECTION_TYPES {
	ORTHOGRAPHIC = 0xFF,
	PERSPECTIVE = 0X00,
};


struct Camera : public Object {
	
	bool active;
	
	float velocity;
	byte projection_type;
	float fov; //field of view
	float aspect_ratio;
	float z_near;
	float z_far;
	float sensitivity; //degrees per second of rotation
	float x_range;
	float y_range;
	glm::mat4 view; //view matrix
	glm::mat4 projection; //projection matrix

	Camera();

	Camera(Transform* t, float v, byte pt, float fov, float xr, float yr, float zn, float zf, float s);

	void UpdateProjection();

	void UpdateView();

	void UpdateMatrices();

	void UpdateSelf(bool rendering) override {
		//std::cout << "UPDATING SELF!" << std::endl;
		UpdateView();
		if (rendering && (active_local && active_global)) {
			glUniformMatrix4fv(current_scene->user->view_matrix_id, 1, GL_FALSE, &view[0][0]);
			glUniform3f(current_scene->user->camera_position_id, t->global.pos[0], t->global.pos[1], t->global.pos[2]);
		}
		
	}
};


#pragma once
#include "transform.h"
#include "byte.h"
#include "object.h"
#include <iostream>
#include <GL/glew.h>
#include "scene.h"
#include "program.h"

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

	Camera(const nlohmann::json& data, Scene* scene);

	Camera(Transform* t, float v, byte pt, float fov, float xr, float yr, float zn, float zf, float s);

	void LookAt(glm::vec3 focus);

	void UpdateProjection();

	void UpdateView();

	void UpdateMatrices();

	void UpdateSelf(bool rendering) override {
		//std::cout << "UPDATING SELF!" << std::endl;
		UpdateView();
		if (rendering && (active_local && active_global)) {
			scene->program->shaders->uniforms.FindAndUpdate("V", view);
			scene->program->shaders->uniforms.FindAndUpdate("camera_position", t->global.pos);
		}
		
	}
};


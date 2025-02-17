#include "camera.h"
#include "cameras.h"
#include "scene.h"
#include <iostream>
Camera::Camera() {
	t = new Transform(glm::vec3(0.f, 0.f, 2.f), this);
	object_type = CAMERA;
	x_range = 8.f;
	y_range = 6.f;
	aspect_ratio = x_range/y_range;
	fov = 60.f;
	projection_type = PERSPECTIVE;
	z_near = 0.1f;
	z_far = 100.f;
	velocity = 5.f; //5 units per second
	sensitivity = 30.f;
	UpdateMatrices();
	LookAt(glm::vec3(0.f, 0.f, 0.f));
}

Camera::Camera(const nlohmann::json& data, Scene* scene) {
	this->scene = scene;
	object_type = CAMERA;
	if (data.contains("transform")) t = new Transform(data["transform"], this);
	else t = new Transform(glm::vec3(0.f, 0.f, 2.f), this);
	projection_type = FetchGLenum(Fetch(data, "projection_type", "perspective"));

	velocity = Fetch(data, "velocity", 5.f);
	fov = Fetch(data, "fov", 60.f);
	x_range = Fetch(data, "x_range", 8.f);
	y_range = Fetch(data, "y_range", 6.f);
	z_near = Fetch(data, "z_near", 0.1f);
	z_far = Fetch(data, "z_far", 100.f);
	sensitivity = Fetch(data, "sensitivity", 30.f);
	active = Fetch(data, "active", true);
}

Camera::Camera(Transform* t, float v, byte pt, float fov, float xr, float yr, float zn, float zf, float s)  : Object(t) {
	object_type = CAMERA;
	velocity = v;
	projection_type = pt;
	this->fov = fov;
	x_range = xr;
	y_range = yr;
	aspect_ratio = x_range/y_range;
	z_near = zn;
	z_far = zf;
	sensitivity = s;
	UpdateMatrices();
}

void Camera::LookAt(glm::vec3 focus) {
	t->LookAt(focus);
	UpdateMatrices();
}

void Camera::UpdateProjection() {
	if (projection_type == PROJECTION_TYPES::ORTHOGRAPHIC) {
		projection = glm::ortho(-(x_range/2.f), x_range/2.f, -(y_range/2.f), y_range/2.f, z_near, z_far);
	} else {
		projection = glm::perspective(glm::radians(fov), aspect_ratio, z_near, z_far);
	}
}

void Camera::UpdateView() {
	glm::vec3 rt = t->global.orn * glm::vec3(1.f, 0.f, 0.f);
	glm::vec3 up = t->global.orn * glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 fwd = t->global.orn * glm::vec3(0.f, 0.f, 1.f);


	view = glm::mat4(1.f);
	view[0][0] = rt.x;	  view[1][0] = rt.y;	  view[2][0] = rt.z;
	view[0][1] = up.x;	  view[1][1] = up.y;	  view[2][1] = up.z;
	view[0][2] = fwd.x;	 view[1][2] = fwd.y;	 view[2][2] = fwd.z;

	view[3][0] = -glm::dot(rt, t->global.pos);
	view[3][1] = -glm::dot(up, t->global.pos);
	view[3][2] = -glm::dot(fwd, t->global.pos);
	
}

void Camera::UpdateMatrices() {
	UpdateProjection();
	UpdateView();
}


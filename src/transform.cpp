#include "transform.h"

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, void* owner, Transform* parent) {
	this->owner = owner;
	this->parent = parent;
	
	local.InitializeOrientation();
	local.pos = position;
	local.scl = scale;
	local.Rotate(rotation[0], rotation[1], rotation[2]);

	UpdateGlobal();
}

Transform::Transform(glm::vec3 position, void* owner, Transform* parent, glm::vec3 rotation, glm::vec3 scale) {
	this->owner = owner;
	this->parent = parent;

	local.InitializeOrientation();
	local.pos = position;
	local.scl = scale;
	local.Rotate(rotation[0], rotation[1], rotation[2]);
	
	UpdateGlobal();
}


Transform::Transform(void* owner, Transform* parent) {
	this->owner = owner;
	this->parent = parent;
	local.InitializeOrientation();
	local.pos = glm::vec3(0.f);
	local.scl = glm::vec3(1.f);
	UpdateGlobal();
}

void Transform::Values::InitializeOrientation() {
	orn = glm::quat(1.f, 0.f, 0.f, 0.f);
}

void Transform::Values::RotateYaw(float angle) { //around y axis
	glm::quat yaw = glm::angleAxis(glm::radians(angle), glm::vec3(0.f, -1.f, 0.f));
	orn = yaw * orn;
}

void Transform::Values::RotatePitch(float angle) { //around x axis
	glm::vec3 rt = orn * glm::vec3(1.f, 0.f, 0.f);
	glm::quat pit = glm::angleAxis(glm::radians(angle), rt);
	orn = pit * orn;
}

void Transform::Values::RotateRoll(float angle) { //around z axis
	glm::vec3 fwd = orn * glm::vec3(0.f, 0.f, -1.f);
	glm::quat rol = glm::angleAxis(glm::radians(angle), fwd);
	orn = rol * orn;
}

void Transform::Values::Rotate(float yaw, float pitch, float roll) {
	RotateYaw(yaw);
	RotatePitch(pitch);
	RotateRoll(roll);
}

void Transform::Values::TranslateForward(float distance, float delta_time) {
	glm::vec3 fwd = orn * glm::vec3(0.f, 0.f, -1.f);
	pos += distance * fwd * delta_time;
}

void Transform::Values::TranslateRight(float distance, float delta_time) {
	glm::vec3 rt = orn * glm::vec3(1.f, 0.f, 0.f);
	pos += distance * rt * delta_time;
}

void Transform::Values::TranslateUp(float distance, float delta_time) {
	glm::vec3 up = orn * glm::vec3(0.f, 1.f, 0.f);
	pos += distance * up * delta_time;
}

void Transform::Values::TranslateGlobalForward(float distance, float delta_time) {
	glm::vec3 fwd = glm::vec3(0.f, 0.f, -1.f);
	pos += distance * fwd * delta_time;
}

void Transform::Values::TranslateGlobalRight(float distance, float delta_time) {
	glm::vec3 rt = glm::vec3(1.f, 0.f, 0.f);
	pos += distance * rt * delta_time;
}

void Transform::Values::TranslateGlobalUp(float distance, float delta_time) {
	glm::vec3 up =  glm::vec3(0.f, 1.f, 0.f);
	pos += distance * up * delta_time;
}

void Transform::Values::TranslateGlobal(float fwd, float rt, float up, float delta_time) {
	TranslateGlobalForward(fwd, delta_time);
	TranslateGlobalRight(rt, delta_time);
	TranslateGlobalUp(up, delta_time);
}

void Transform::Values::Translate(float fwd, float rt, float up, float delta_time) {
	TranslateForward(fwd, delta_time);
	TranslateRight(rt, delta_time);
	TranslateUp(up, delta_time);
}

void Transform::Values::UniformScale(float factor) {
	scl *= factor;
}

void Transform::Values::SetValue(glm::vec3& val_to_set, glm::vec3 intended_val) {
	val_to_set = intended_val;
}

void Transform::Values::SetQuatValue(glm::quat& val_to_set, glm::quat intended_val) {
	val_to_set = intended_val;
}

void Transform::UpdateGlobal() {
	if (parent == nullptr) {
		global = local;
	} else {
		global.pos = parent->global.pos + (parent->global.orn * local.pos);

		global.orn = parent->global.orn * local.orn;

		global.scl = parent->global.scl * local.scl;
	}
}

void Transform::SetParent(Transform* parent) {
	this->parent = parent;
	UpdateGlobal();
}

void Transform::UpdateLocalPositionFromGlobal(glm::vec3 old_global) {
	glm::vec3 local_adjustment = global.pos - old_global;
	local.pos += local_adjustment;
}
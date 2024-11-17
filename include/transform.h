#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
	struct Values {
		glm::vec3 pos;
		glm::quat orn;
		glm::vec3 scl;

		void InitializeOrientation();
		void RotateYaw(float angle);
		void RotatePitch(float angle);
		void RotateRoll(float angle);
		void Rotate(float yaw, float pitch, float roll);
		
		void TranslateForward(float distance, float delta_time);
		void TranslateRight(float distance, float delta_time);
		void TranslateUp(float distance, float delta_time);
		void Translate(float fwd, float rt, float up, float delta_time);

		void TranslateGlobalForward(float distance, float delta_time);
		void TranslateGlobalRight(float distance, float delta_time);
		void TranslateGlobalUp(float distance, float delta_time);
		void TranslateGlobal(float fwd, float rt, float up, float delta_time);

		void SetValue(glm::vec3& val_to_set, glm::vec3 intended_val);
		void SetQuatValue(glm::quat& val_to_set, glm::quat intended_val);

		void UniformScale(float factor);
	};

	Values local;
	Values global;
	
	Transform* parent; //parent transform
	void* owner; //object described by the transform

	Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale = glm::vec3(1.f), void* owner = nullptr, Transform* parent = nullptr);
	Transform(glm::vec3 position, void* owner, Transform* parent = nullptr, glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f));
	Transform(void* owner, Transform* parent = nullptr);
	void UpdateGlobal();
	void SetParent(Transform* parent);
	void UpdateLocalPositionFromGlobal(glm::vec3 old_global);
	//TODO setposition and setglobalposition along with scl and rot
	
};
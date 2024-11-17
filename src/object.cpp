#include "object.h"
#include <iostream>
#include "mesh.h"
#include "camera.h"
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
Object::Object() {
	object_type = OBJECT;
	active_global = true;
	active_local = true;
}

Object::Object(Transform* t) {
	this->t = t;
	object_type = OBJECT;
	active_global = true;
	active_local = true;
}

void Object::UpdateTree(bool rendering) {
	t->UpdateGlobal();
	UpdateSelf(rendering);
	if (children.size() > 0) {
		for (auto& child : children) {
			child->UpdateTree(rendering);
		}
	}

}

void Object::DrawTree(Camera* camera) {
	Draw(camera);
	if (children.size() > 0) {
		for (auto& child : children) {
			child->DrawTree(camera);
		}
	}
}

void Object::PrintTree(int lvl) {
	for (int i = 0; i < lvl + 1; i++) {
		std::cout << "-";
	}
	std::cout << this->name << std::endl;
	std::cout << "POSITION:" << std::endl;
	std::cout << "Local: " << glm::to_string(t->local.pos) << "\t Global: " << glm::to_string(t->global.pos) << std::endl;
	std::cout << "ORIENTATION:" << std::endl;
	std::cout << "Local: " << glm::to_string(t->local.orn) << "\t Global: " << glm::to_string(t->global.orn) << std::endl;
	if (children.size() > 0) {
		for (auto& child : children) {
			child->PrintTree(lvl + 1);
		}
	}
}

void Object::AttachChild(Object* child) { //TODO make bool for successful attachment, same with other ones
	if (child == nullptr) return;
	children.push_back(child);
	child->parent = this;
	child->t->parent = this->t;
	UpdateTree();
}

void Object::DetatchChild(Object* child) {
	if (child == nullptr) return;
	
	for (int i = 0; i < children.size(); i++) {
		if (children[i] == child) children.erase(children.begin() + i);
	}
	child->parent = nullptr;
	child->t->parent = nullptr;
	UpdateTree();
	child->UpdateTree();
}

Object* Object::GetChildByNameTree(std::string name) {
	Object* found_child = nullptr;
	for (auto& child : children) {
		if (child->name == name) return child; //as soon as we get a hit we return
	}
	for (auto& child : children) {
		found_child = child->GetChildByNameTree(name);
		if (found_child != nullptr) break; //immediately break to return found child;
	}
	return found_child;
}

void Object::SetActiveLocalTree(bool active) {
	active_local = active;
	if (parent == nullptr) active_global = active;
	for (auto& child : children) {
		child->SetActiveGlobalTree(active);
	}
}

void Object::SetActiveGlobalTree(bool active) {
	active_global = active;
	for (auto& child : children) {
		if (active_local || !active) child->SetActiveGlobalTree(active);
	}
}
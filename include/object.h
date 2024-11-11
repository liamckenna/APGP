#pragma once
#include "transform.h"
#include <vector>
#include <string>
#include "byte.h"
struct Scene;
struct Camera;
enum OBJECT_TYPE {
	OBJECT	= 0x00,
	MESH	= 0X01,
	CAMERA	= 0X02,
	LIGHT	= 0X03
};

struct Object {
	Transform* t;
	
	std::string name;
	
	byte object_type;

	Object* parent;
	std::vector<Object*> children;

	bool active_global;
	bool active_local;

	Scene* current_scene;

	void UpdateTree(bool rendering = false);

	void AttachChild(Object* child);

	void DetatchChild(Object* child);

	Object(Transform* t);
	Object();

	void PrintTree(int lvl);

	virtual void UpdateSelf(bool rendering) {
		
	}

	void DrawTree(Camera* camera);

	virtual void Draw(Camera* camera) {

	}

	Object* GetChildByNameTree(std::string name);

	void SetActiveLocalTree(bool active);

	void SetActiveGlobalTree(bool active);
};
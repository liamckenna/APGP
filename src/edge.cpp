#include "edge.h"
#include "mesh.h"
#include "scene.h"
#include <iostream>
Edge::Edge(Vertex* v[2]) {
	this->v[0] = v[0];
	this->v[1] = v[1];
};

Edge::Edge(Vertex* v0, Vertex* v1, Mesh* m) {
	this->m = m;
	AssignMaterial(m->current_scene->current_material);
	//if (m->name == "grid" && mtl == m->current_scene->default_material) std::cout << "HELP ME!" << std::endl;
	v[0] = v0;
	v[1] = v1;
	c = Color();
}

Vertex*& Edge::operator[](int index) {
	return v[index];
};



void Edge::AssignMaterial(Material* material) {
	if (material == nullptr) {
		mtl = m->current_scene->default_material;
	} else {
		mtl = material;
	}
	mtl->edges++;
}
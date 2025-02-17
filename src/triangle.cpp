#include "triangle.h"
#include <iostream>
#include <string>
#include "mesh.h"
#include "scene.h"
#include "materials.h"

Triangle::Triangle(Vertex* v0, Vertex* v1, Vertex* v2, bool precalculated_normals, Mesh* m) {
	AssignMaterial(m->scene->materials->current);
	this->m = m;
	this->v[0] = v0;
	this->v[1] = v1;
	this->v[2] = v2;
	this->precalculated_normals = precalculated_normals;
	CalculateTransform();
	CreateEdgesFromVertices();
	CalculateNormal();
	if (!precalculated_normals) {
		UpdateVertexNormals();
	}
	
}


void Triangle::RetrieveVerticesFromEdges() {
	v[0] = e_01->v[0];
	v[1] = e_12->v[0];
	v[2] = e_20->v[0];
}

void Triangle::CreateEdgesFromVertices() {
	e_12 = new Edge(v[1], v[2], m);
	e_20 = new Edge(v[2], v[0], m);
	e_01 = new Edge(v[0], v[1], m);
}

void Triangle::SetColor(COLORS color_name) {
	c = Color(color_name);
}

void Triangle::SetColor(Color c) {
	this->c = c;
}

void Triangle::CalculateNormal(bool verbose) {
	glm::vec3 ev_01 = glm::vec3(v[1]->t->local.pos - v[0]->t->local.pos);
	glm::vec3 ev_12 = glm::vec3(v[2]->t->local.pos - v[1]->t->local.pos);

	glm::vec3 norm = glm::cross(ev_12, ev_01);

	glm::vec3 unit = glm::normalize(norm);

	if (glm::length(norm) > 0.0f) {
		unit = glm::normalize(norm);
	} else {
		unit = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	n[0] = unit[0];
	n[1] = unit[1];
	n[2] = unit[2];
	
	if (verbose) {
		std::cout << "Normal: (" << n[0] << ", " << n[1] << ", " << n[2] << ")" << std::endl;
		std::cout << "Mesh: " << this->m->name << std::endl;
	}
}

void Triangle::CalculateTransform() {
	glm::vec3 tpos = (v[0]->t->local.pos + v[1]->t->local.pos + v[2]->t->local.pos) / 3.f;
	this->t = new Transform(tpos, this, m->t);
}

void Triangle::UpdateVertexNormals() {
	for (auto& vertex : this->v) {
		vertex->n.n += this->n.n;
	}
}

void Triangle::AssignMaterial(Material* material) {
	if (material == nullptr) {
		mtl = m->scene->materials->dval;
	} else {
		mtl = material;
	}
	mtl->tris++;
}
#pragma once
#include "vertex.h"
#include "transform.h"
#include "material.h"
#include "mesh.h"
struct Mesh;

struct Edge {
		
	Vertex* v[2];
	Mesh* m;
	Color c;
	Material* mtl;

	Edge(Vertex* v[2]);

	Edge(Vertex* v0, Vertex* v1, Mesh* m);

	Vertex*& operator[](int index);

	void AssignMaterial(Material* material);

};
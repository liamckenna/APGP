#pragma once

#include "transform.h"
#include "normal.h"
#include "vertex.h"
#include "edge.h"
#include "material.h"
struct Mesh;
struct Edge;
struct Triangle {
	Transform* t;
	Normal n;
	Vertex* v[3]; //vertices
	int v_id[3];
	Color c;
	Mesh* m;
	Material* mtl;
	bool precalculated_normals;
	
	union {
		struct {
			Edge* e_12; //e[0] is the edge perpendicular to v[0]
			Edge* e_20; //e[1] is the edge perpendicular to v[1]
			Edge* e_01; //e[2] is the edge perpendicular to v[2]
		};
		Edge* e[3]; //edges
	};

	Triangle(Vertex* v0, Vertex* v1, Vertex* v2, bool precalculated_normals, Mesh* m);

	void RetrieveVerticesFromEdges();
	void CreateEdgesFromVertices();
	void SetColor(COLORS color_name);
	void SetColor(Color c);
	void CalculateNormal(bool verbose = false);
	void CalculateTransform();
	void UpdateVertexNormals();
	void AssignMaterial(Material* material);
};
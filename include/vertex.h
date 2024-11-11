#pragma once
#include <glm/vec2.hpp>
#include "transform.h"
#include "color.h"
#include "normal.h"
#include <vector>

//TODO: CREATE Texture_Map OBJECT
typedef void Texture_Map;  //FOR NOW
struct Mesh;
struct Vertex {
	glm::vec3 position;
	Transform* t;
	Color c;
	Normal n;
	int id;
	Mesh* m;

	
	glm::vec2 tex_coord; //texture coordinate

	Vertex();
	Vertex(Transform* t, Color c, Normal n = Normal());
	Vertex(Transform* t, Normal n, Color c = Color());
	Vertex(Transform* t);
	Vertex(glm::vec3 p, Transform* parent = nullptr);
	Vertex(glm::vec3 p, int id);
	Vertex(float p0, float p1, float p2);
	Vertex(float p0, float p1, float p2, COLORS color_name);
};

struct FlattenedVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec4 color;
	glm::vec2 tex_coord;
	int material_index;
};

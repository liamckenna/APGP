#include "mesh.h"
#include <iostream>
#include "scene.h"
#include "materials.h"


void Mesh::SetupBuffers() {
	
	GenerateBuffers();
	PopulateBuffers(Flatten());

}

void Mesh::GenerateBuffers() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
}

void Mesh::PopulateBuffers(std::vector<FlattenedVertex> flattened_vertices) {

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, flattened_vertices.size() * sizeof(FlattenedVertex), flattened_vertices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, color));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, tex_coord));
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(4, 1, GL_INT, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, material_index));
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(5, 1, GL_INT, sizeof(FlattenedVertex), (void*)offsetof(FlattenedVertex, draw_mode));
	glEnableVertexAttribArray(5);

	glBindVertexArray(0);
}


std::vector<FlattenedVertex> Mesh::Flatten() {
	std::vector<FlattenedVertex> flatVertices;

	if (draw_mode == GL_TRIANGLES) {
		for (const auto& triangle : this->triangles) {
			for (const auto& vertex: triangle->v) {
				FlattenedVertex flatVertex;
				flatVertex.position = vertex->t->global.pos;
				flatVertex.normal = vertex->n.n;
				flatVertex.color = triangle->c.ToVec4();
				flatVertex.tex_coord = vertex->tex_coord;
				flatVertex.material_index = triangle->mtl->idx;
				flatVertex.draw_mode = GetDrawModeIdx();
				flatVertices.push_back(flatVertex);
			}
		}
	} else if (draw_mode == GL_LINES) {
		for (const auto& edge : this->edges) {
			for (const auto& vertex: edge->v) {
				FlattenedVertex flatVertex;
				flatVertex.position = vertex->t->global.pos;
				flatVertex.normal = vertex->n.n;
				flatVertex.color = edge->c.ToVec4();
				flatVertex.tex_coord = vertex->tex_coord;
				flatVertex.material_index = edge->mtl->idx;
				flatVertex.draw_mode = GetDrawModeIdx();
				flatVertices.push_back(flatVertex);
			}
		}
		//std::cout << "finished flattening edges" << std::endl;
		//std::cout << flatVertices.size() << std::endl;
	}

	
	return flatVertices;
}
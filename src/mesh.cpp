#include "mesh.h"
#include <iostream>
#include "scene.h"
Mesh::Mesh() {
	default_color = Color(COLORS::DEFAULT);
	tri_color = default_color;
	model = glm::mat4(1.f);
	object_type = MESH;
}

Mesh::Mesh(Color color) {
	default_color = color;
	tri_color = default_color;
	model = glm::mat4(1.f);
	object_type = MESH;
}

Mesh::Mesh(COLORS color_name) {
	default_color = Color(color_name);
	tri_color = default_color;
	model = glm::mat4(1.f);
	object_type = MESH;
}

std::vector<FlattenedVertex> Mesh::flattenVertices() {
	std::vector<FlattenedVertex> flatVertices;

	if (draw_mode == GL_TRIANGLES) {
		for (const auto& triangle : this->triangles) {
			for (const auto& vertex: triangle->v) {
				FlattenedVertex flatVertex;
				flatVertex.position = vertex->t->global.pos;
				flatVertex.normal = vertex->n.n;
				flatVertex.color = triangle->c.ToVec4();
				flatVertex.tex_coord = vertex->tex_coord;
				flatVertex.material_index = triangle->mtl->index;
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
				flatVertex.material_index = edge->mtl->index;
				flatVertices.push_back(flatVertex);
			}
		}
		std::cout << "finished flattening edges" << std::endl;
		std::cout << flatVertices.size() << std::endl;
	}

	
	return flatVertices;
}

std::vector<Vertex> Mesh::GetVertexArray() {
	std::vector<Vertex> verts;
	// for (int i = 0; i < vertices.size(); i++) {
	// 	verts.push_back(*vertices[i]);
	// }
	return verts;
}

void Mesh::InsertVertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& tex_coord) {
	vertex_positions.push_back(position);
	vertex_normals.push_back(normal);
	texture_coords.push_back(tex_coord);
}

void Mesh::InsertTriangle(int vp0, int vp1, int vp2) {
	triangles.push_back(new Triangle(vertices[vp0], vertices[vp1], vertices[vp2], false, this));
}

void Mesh::InsertTriangleWithNormals(int vp0, int vp1, int vp2, int vn0, int vn1, int vn2) {
	Vertex* v0 = new Vertex(vertex_positions[vp0]);
	v0->n = vertex_normals[vn0];
	Vertex* v1 = new Vertex(vertex_positions[vp1]);
	v1->n = vertex_normals[vn1];
	Vertex* v2 = new Vertex(vertex_positions[vp2]);
	v2->n = vertex_normals[vn2];
	triangles.push_back(new Triangle(v0, v1, v2, true, this));
}

void Mesh::InsertTriangleWithTexCoords(int vp0, int vp1, int vp2, int vt0, int vt1, int vt2) {

	Vertex* v0 = new Vertex(vertex_positions[vp0], this->t);
	v0->tex_coord = texture_coords[vt0];
	Vertex* v1 = new Vertex(vertex_positions[vp1], this->t);
	v1->tex_coord = texture_coords[vt1];
	Vertex* v2 = new Vertex(vertex_positions[vp2], this->t);
	v2->tex_coord = texture_coords[vt2];
	triangles.push_back(new Triangle(v0, v1, v2, false, this));
	
}

void Mesh::InsertTriangleWithAllData(int vp0, int vp1, int vp2, int vn0, int vn1, int vn2, int vt0, int vt1, int vt2) {
	Vertex* v0 = new Vertex(vertex_positions[vp0], this->t);
	v0->n = vertex_normals[vn0];
	v0->tex_coord = texture_coords[vt0];
	Vertex* v1 = new Vertex(vertex_positions[vp1], this->t);
	v1->n = vertex_normals[vn1];
	v1->tex_coord = texture_coords[vt1];
	Vertex* v2 = new Vertex(vertex_positions[vp2], this->t);
	v2->n = vertex_normals[vn2];
	v2->tex_coord = texture_coords[vt2];
	triangles.push_back(new Triangle(v0, v1, v2, true, this));
}

void Mesh::InsertEdge(int v0, int v1) {
	edges.push_back(new Edge(vertices[v0], vertices[v1], this));
}

void Mesh::SetTriColor(COLORS color_name) {
	tri_color = Color(color_name);
}

void Mesh::SetTriColor(Color c) {
	tri_color = c;
}

std::vector<glm::vec4> Mesh::GetTriangleColors() {
	std::vector<glm::vec4> tri_colors;
	for (int i = 0; i < triangles.size(); i++) {
		tri_colors.push_back(triangles[i]->c.ToVec4());
	}
	return tri_colors;
}

std::vector<glm::vec3> Mesh::GetTriangleNormals() {
	std::vector<glm::vec3> tri_normals;
	for (int i = 0; i < triangles.size(); i++) {
		tri_normals.push_back(triangles[i]->n.n);
	}
	return tri_normals;
}

void Mesh::NormalizeVertexNormals() {

	int no_need = 0;
	for (int i = 0; i < vertices.size(); i++) {
		glm::vec3 b4 =  vertices[i]->n.n;
		vertices[i]->n.n = glm::normalize(vertices[i]->n.n);
		if (name == "pokeball" && b4 == vertices[i]->n.n) {
			no_need++;
		}
	}
}

void Mesh::UpdateModelMatrix() {
	model = glm::mat4(1.f);
	
	model = glm::translate(model, this->t->global.pos);
	
	model *= glm::mat4_cast(this->t->global.orn);

	model = glm::scale(model, this->t->global.scl);
}

void Mesh::SetCurrentMaterial(const std::string& material_name) {
	for (Material* material : current_scene->materials) {
		if (material->name == material_name) {
			current_scene->current_material = material;
			break;
		}
	}
}


void Mesh::SetDrawMode(const std::string& mode) {
	if (mode == "GL_POINTS") draw_mode = GL_POINTS;
	else if (mode == "GL_LINES") draw_mode = GL_LINES;
	else if (mode == "GL_LINE_STRIP") draw_mode = GL_LINE_STRIP;
	else if (mode == "GL_LINE_LOOP") draw_mode = GL_LINE_LOOP;
	else if (mode == "GL_TRIANGLES") draw_mode = GL_TRIANGLES;
	else if (mode == "GL_TRIANGLE_STRIP") draw_mode = GL_TRIANGLE_STRIP;
	else if (mode == "GL_TRIANGLE_FAN") draw_mode = GL_TRIANGLE_FAN;
}

void Mesh::SetDefaultDrawMode() {
	draw_mode = current_scene->default_draw_mode;
}
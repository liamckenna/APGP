#pragma once
#include <vector>
#include <GL/glew.h>
#include "triangle.h"
#include "color.h"
#include <string>
#include "material.h"
#include "edge.h"
#include "object.h"
#include "scene.h"
#include "buffers.h"
#include "camera.h"
struct Edge;
struct Triangle;
struct Mesh : public Object {
	std::vector<Vertex*> vertices;
	std::vector<glm::vec3> vertex_positions;
	std::vector<glm::vec3> vertex_normals;
	std::vector<glm::vec2> texture_coords;
	std::vector<Triangle*> triangles;
	std::vector<Edge*> edges;
	std::vector<Material*> materials;

	glm::mat4 model; //model matrix

	Color default_color;
	
	Color tri_color;
	
	GLuint draw_mode;

	int idx;

	bool dif_only;

	bool verbose;
	bool visible;

	Mesh();
	Mesh(Color color);
	Mesh(COLORS color_name);

	std::vector<FlattenedVertex> flattenVertices();
	std::vector<Vertex> GetVertexArray();
	void InsertVertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& tex_coord);
	void InsertTriangle(int vp0, int vp1, int vp2);
	void InsertTriangleWithNormals(int vp0, int vp1, int vp2, int vn0, int vn1, int vn2);
	void InsertTriangleWithTexCoords(int vp0, int vp1, int vp2, int vt0, int vt1, int vt2);
	void InsertTriangleWithAllData(int vp0, int vp1, int vp2, int vn0, int vn1, int vn2, int vt0, int vt1, int vt2);

	void InsertEdge(int v0, int v1);

	void SetTriColor(COLORS color_name);
	void SetTriColor(Color c);
	std::vector<glm::vec4> GetTriangleColors();
	std::vector<glm::vec3> GetTriangleNormals();
	void NormalizeVertexNormals();
	void UpdateModelMatrix();

	void SetCurrentMaterial(const std::string& material_name);
	void SetDrawMode(const std::string& draw_mode);
	void SetDefaultDrawMode();

	void UpdateSelf(bool rendering) override {
		UpdateModelMatrix();
	}

	void Draw(Camera* camera) override {
		
		if (!visible || !(active_local && active_global)) return;
		UpdateModelMatrix();
		glUniformMatrix4fv(current_scene->user->matrix_id, 1, GL_FALSE, &(camera->projection * camera->view * model)[0][0]);
		glUniformMatrix4fv(current_scene->user->model_matrix_id, 1, GL_FALSE, &model[0][0]);
		glBindVertexArray(current_scene->buffers->vertex_arrays[idx]);
		if (draw_mode == GL_TRIANGLES) glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3);
		else if (draw_mode == GL_LINES) glDrawArrays(GL_LINES, 0, edges.size() * 2);

	
	}
};
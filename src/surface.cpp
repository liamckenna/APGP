#include "surface.h"
#include <iostream>
#include "scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#undef GLM_ENABLE_EXPERIMENTAL

void Surface::SetupBuffers() {
	GenerateBuffers();
	std::cout << "buffers generated" << std::endl;
	PopulateBuffers(Flatten());
	std::cout << "buffers populated" << std::endl;
	CreatePatches();
	std::cout << "patches created" << std::endl;
}

void Surface::GenerateBuffers() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &ssbo);
}

void Surface::PopulateBuffers(std::vector<FlatVertex> flat_vertices) {

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, flat_vertices.size() * sizeof(FlatVertex), flat_vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FlatVertex), (void*)offsetof(FlatVertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FlatVertex), (void*)offsetof(FlatVertex, texcoord));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FlatVertex), (void*)offsetof(FlatVertex, normal));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, material_index.size() * sizeof(int), material_index.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_BINDING_POINT, ssbo);
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(int), (void*)0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindVertexArray(0);
}

std::vector<FlatVertex> Surface::Flatten() {
	std::vector<FlatVertex> flat_vertices;
	flat_vertices.reserve(vertices.size());

	for (const Vertex& v : vertices) {
		FlatVertex fv;
		fv.position = v.position;
		fv.texcoord = v.texcoord;
		fv.normal =	v.normal;
		flat_vertices.push_back(fv);
	}

	return flat_vertices;
}

void Surface::CreatePatches()
{

	
	 
	num_patches = vertices.size() / patch_size;

	// update bounding box
	bbox[0] = glm::vec3(10000.f, 10000.f, 10000.f);
	bbox[1] = glm::vec3(-10000.f, -10000.f, -10000.f);

	for (int i = 0; i < vertices.size(); i++)
	{
		bbox[0] = glm::min(bbox[0], vertices[i].position);
		bbox[1] = glm::max(bbox[1], vertices[i].position);
	}


	// calculate connectivity
	//calConnectivityOfPatches

	float eps = 0.00001;
	int edge_point_idices[4][4] = { {0,1,2,3},		// top row
									{12,13,14,15},	// bottom row
									{0,4,8,12},		// left column
									{3,7,11,15}		// right column
	};

	int num_patches = indices.size() / patch_size;

	std::vector<glm::ivec4> conn(num_patches, glm::ivec4(-1, -1, -1, -1));

	std::vector<glm::vec3> vertex_table;

	// array of <patch_id, edge_id> to record which edge this vertex belong to
	std::vector<std::vector<std::pair<int, int> > > vertices_location;


	for (int patch_idx = 0; patch_idx < num_patches; patch_idx++)
	{
		for (int edge_idx = 0; edge_idx < 4; edge_idx++)
		{
			for (int vidx = 0; vidx < 4; vidx++)
			{
				glm::vec3 pos = vertices[indices[patch_idx * patch_size + edge_point_idices[edge_idx][vidx]]].position;
				// glm::vec4 pos(pos_ptr[0], pos_ptr[1], pos_ptr[2], pos_ptr[3]);

				int found_idx = -1;
				// search pos in vertex table
				for (int i = 0; i < vertex_table.size(); i++)
				{
					if (glm::distance(vertex_table[i], pos) < eps)
					{
						found_idx = i;
						break;
					}
				}

				if (found_idx == -1) // not found
				{
					vertex_table.push_back(pos);
					vertices_location.push_back(std::vector<std::pair<int, int> >());
					found_idx = vertex_table.size() - 1;
				}

				vertices_location[found_idx].push_back(std::make_pair(patch_idx, edge_idx));

				// cout << "v:" << found_idx << " patch_id:" << patch_idx << " edge_id:" << edge_idx << endl;
			} // for each vidx
		} // for each edge_idx
	} // for each patch_idx

	// connect each pair of patches connect by a point
	for (int vidx = 0; vidx < vertex_table.size(); vidx++)
	{
		// only deal with point within the edge
		if (vertices_location[vidx].size() != 2) {
			continue;
		}

		for (int i = 0; i < vertices_location[vidx].size(); i++)
		{
			std::pair<int, int> location = vertices_location[vidx][i];
			for (int j = i + 1; j < vertices_location[vidx].size(); j++)
			{

				std::pair<int, int> next_location = vertices_location[vidx][j];
				// prevent self connection caused by corner vertex
				if (location.first == next_location.first) {
					continue;
				}
				
				if (conn[location.first][location.second] == -1)
				{
					conn[location.first][location.second] = next_location.first;
				}
				else
				{
					assert(conn[location.first][location.second] == next_location.first);
				}


				if (conn[next_location.first][next_location.second] == -1)
				{
					conn[next_location.first][next_location.second] = location.first;
				}
				else
				{
					assert(conn[next_location.first][next_location.second] == location.first);
				}
			}

		}
	}

	patches = conn;


	glGenTextures(1, &patch_tess_level_texture);
	glBindTexture(GL_TEXTURE_1D, patch_tess_level_texture);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, num_patches, 0, GL_RED, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_1D, 0);

	glGenTextures(1, &connectivity_texture);
	glBindTexture(GL_TEXTURE_1D, connectivity_texture);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32I, num_patches, 0, GL_RGBA_INTEGER, GL_INT, NULL);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_1D, 0);

	glBindTexture(GL_TEXTURE_1D, connectivity_texture);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, patches.size(), GL_RGBA_INTEGER, GL_INT, &patches[0]);
}
#include "object_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include <fstream>
#include <sstream>
#include <filesystem>
#include "object_parser.h"
#include "material.h"
#include "scene.h"


void ObjectParser::ParseObjFile(const std::string& filepath, Mesh*& mesh) {
	std::string absolute_filepath = std::filesystem::current_path().string() + filepath;
	std::ifstream file(absolute_filepath);

	if (!file.is_open()) {
		std::cerr << "Failed to open .obj file: " << filepath << std::endl;
		return;
	}

	std::string material_library;
	std::string current_material;
	std::string line;

	while (std::getline(file, line)) {
		if (line.substr(0, 7) == "mtllib ") { //extract material library file name
			
			material_library = line.substr(7);
			material_library = "/data/materials/" + material_library;
			ParseMtlFile(material_library, mesh);
		} else if (line.substr(0, 7) == "usemtl ") { //extract material name to apply to subsequent faces
			current_material = line.substr(7);
			mesh->SetCurrentMaterial(current_material);
			if (mesh->name == "grid") std::cout << "Current material: " << mesh->current_scene->current_material->name << std::endl;
		} else if (line.substr(0, 2) == "v ") {
			ParseVertex(line, mesh);
		} else if (line.substr(0, 3) == "vn ") {
			ParseNormal(line, mesh, mesh->vertex_normals.size());
		} else if (line.substr(0, 3) == "vt ") {
			ParseTextureCoord(line, mesh, mesh->texture_coords.size());
		} else if (line.substr(0, 2) == "f ") {
			ParseFace(line, mesh);
		} else if (line.substr(0, 2) == "l ") {
			ParseEdge(line, mesh);
		} 
	}

	mesh->NormalizeVertexNormals();
	mesh->SetCurrentMaterial(mesh->current_scene->default_material->name);
	file.close();
}


void ObjectParser::ParseVertex(const std::string& line, Mesh*& mesh) {
	std::istringstream s(line.substr(2));
	float x, y, z;
	s >> x >> y >> z;

	glm::vec3 v = glm::vec3(x, y, z);
	Vertex* vertex = new Vertex(v, mesh->vertices.size());
	mesh->vertex_positions.push_back(v);
	mesh->vertices.push_back(vertex);
}

void ObjectParser::ParseNormal(const std::string& line, Mesh*& mesh, int idx) {
	std::istringstream s(line.substr(3));
	float nx, ny, nz;
	s >> nx >> ny >> nz;
	glm::vec3 vn = glm::vec3(nx, ny, nz);
	mesh->vertex_normals.push_back(vn);
}

void ObjectParser::ParseTextureCoord(const std::string& line, Mesh*& mesh, int idx) {
	std::istringstream s(line.substr(3));
	float u, v;
	s >> u >> v;

	glm::vec2 vt = glm::vec2(u, v);
	mesh->texture_coords.push_back(vt);
}

void ObjectParser::ParseFace(const std::string& line, Mesh*& mesh) {
	std::istringstream s(line.substr(2));
	std::string token;
	std::vector<int> vertex_indices;
	std::vector<int> normal_indices;
	std::vector<int> texture_indices;
	

	while (s >> token) {
		std::vector<std::string> face_components = Split(token, '/');
		int vi = std::stoi(face_components[0]) - 1;  //.obj indices are 1-based
		vertex_indices.push_back(vi);

		if (face_components.size() > 1 && !face_components[1].empty()) {
			int vti = std::stoi(face_components[1]) - 1;
			texture_indices.push_back(vti);
		}
		if (face_components.size() > 2) {
			int vni = std::stoi(face_components[2]) - 1;
			normal_indices.push_back(vni);
		}
	}


	
	if (vertex_indices.size() >= 3) {
		
		for (size_t i = 1; i < vertex_indices.size() - 1; ++i) {  //generalized for any n-gon starting with the first triangle
			
			if (normal_indices.size() > 0 && texture_indices.size() > 0) { //check if both normal and texture indices are available
				mesh->InsertTriangleWithAllData(vertex_indices[0], vertex_indices[i + 1], vertex_indices[i],
												normal_indices[0], normal_indices[i + 1], normal_indices[i],
												texture_indices[0], texture_indices[i + 1], texture_indices[i]);
				if (i % 2 == 1) { //if we are on an odd cycle
					mesh->InsertEdgeWithTexCoords(	vertex_indices[0], vertex_indices[i],
													texture_indices[0], texture_indices[i]);
					mesh->InsertEdgeWithTexCoords(	vertex_indices[i], vertex_indices[i + 1], 
													texture_indices[i], texture_indices[i + 1]);
				} else { //even cycle
					mesh->InsertEdgeWithTexCoords(	vertex_indices[0], vertex_indices[i + 1],
													texture_indices[0], texture_indices[i + 1]);
					mesh->InsertEdgeWithTexCoords(	vertex_indices[i + 1], vertex_indices[i],
													texture_indices[i + 1], texture_indices[i]);
				}
				
			}
			else if (normal_indices.size() > 0) { //check if only normal indices are available
				mesh->InsertTriangleWithNormals(vertex_indices[0], vertex_indices[i + 1], vertex_indices[i],
												normal_indices[0], normal_indices[i + 1], normal_indices[i]);
			} else if (texture_indices.size() > 0) { //check if only texture indices are available
				mesh->InsertTriangleWithTexCoords(vertex_indices[0], vertex_indices[i + 1], vertex_indices[i],
												 texture_indices[0], texture_indices[i + 1], texture_indices[i]);
			} else { //only vertex data is available
				mesh->InsertTriangle(vertex_indices[0], vertex_indices[i + 1], vertex_indices[i]);
			}
		}
	}
}

void ObjectParser::ParseEdge(const std::string& line, Mesh*& mesh) {
	std::istringstream s(line.substr(2));
	std::string token;
	std::vector<int> vertex_indices;

	while (s >> token) {
		std::vector<std::string> face_components = Split(token, '/');
		int vi = std::stoi(face_components[0]) - 1;
		vertex_indices.push_back(vi);
	}

	if (vertex_indices.size() >= 2) {
		for (size_t i = 1; i < vertex_indices.size(); i++) {
			mesh->InsertEdge(vertex_indices[i-1],vertex_indices[i]);
		}
	}
	
}



std::vector<std::string> ObjectParser::Split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

void ObjectParser::ParseMtlFile(const std::string& filepath, Mesh*& mesh) {
	std::string absolute_filepath = std::filesystem::current_path().string() + filepath;
	std::cout << "MTL FILEPATH: " << absolute_filepath << std::endl;
	std::ifstream file(absolute_filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open .mtl file: " << absolute_filepath << std::endl;
		return;
	}

	Material* material = nullptr;
	std::string line;

	while (std::getline(file, line)) {
		
		if (line.substr(0, 7) == "newmtl ") {
			if (material != nullptr) mesh->current_scene->materials.push_back(material);
			material = new Material();  //start a new material
			material->name = line.substr(7);
			material->index = mesh->current_scene->materials.size();
		} else if (line.substr(0, 3) == "Kd ") { //diffuse color
			std::istringstream s(line.substr(3));
			s >> material->colors.dif.x >> material->colors.dif.y >> material->colors.dif.z;
		} else if (line.substr(0, 3) == "Ka ") { //ambient color
			std::istringstream s(line.substr(3));
			s >> material->colors.amb.x >> material->colors.amb.y >> material->colors.amb.z;
		} else if (line.substr(0, 3) == "Ks ") { //specular color
			std::istringstream s(line.substr(3));
			s >> material->colors.spc.x >> material->colors.spc.y >> material->colors.spc.z;
		} else if (line.substr(0, 3) == "Ke ") { //emissive color
			std::istringstream s(line.substr(3));
			s >> material->colors.ems.x >> material->colors.ems.y >> material->colors.ems.z;
		} else if (line.substr(0, 7) == "map_Kd ") { //diffuse texture (albedo)
			std::string texture_file = line.substr(7);
			material->textures.dif = ParseTxtFile(texture_file, mesh, material, TXT_DIFFUSE);
		} else if (line.substr(0, 7) == "map_Ks ") { //specular texture
			std::string texture_file = line.substr(7);
			material->textures.spc = ParseTxtFile(texture_file, mesh, material, TXT_SPECULAR);
		} else if (line.substr(0, 8) == "map_bump" || line.substr(0, 9) == "map_Bump ") { //bump map (normal map)
			std::string texture_file = line.substr(line.find(' ') + 1);
			material->textures.bmp = ParseTxtFile(texture_file, mesh, material, TXT_BUMP);
		} else if (line.substr(0, 7) == "map_Ns ") { //roughness texture
			std::string texture_file = line.substr(7);
			material->textures.rgh = ParseTxtFile(texture_file, mesh, material, TXT_ROUGHNESS);
		} else if (line.substr(0, 5) == "map_d ") { //opacity (alpha) texture
			std::string texture_file = line.substr(5);
			material->textures.opc = ParseTxtFile(texture_file, mesh, material, TXT_OPACITY);
		} else if (line.substr(0, 9) == "map_disp ") { //displacement map
			std::string texture_file = line.substr(9);
			material->textures.dsp = ParseTxtFile(texture_file, mesh, material, TXT_DISPLACEMENT);
		} else if (line.substr(0, 9) == "map_aoc ") { //ambient occlusion texture
			std::string texture_file = line.substr(9);
			material->textures.aoc = ParseTxtFile(texture_file, mesh, material, TXT_AMBIENTOCCLUSION);
		} else if (line.substr(0, 7) == "map_Ke ") { //emissive texture
			std::string texture_file = line.substr(7);
			material->textures.ems = ParseTxtFile(texture_file, mesh, material, TXT_EMISSIVE);
		} else if (line.substr(0, 3) == "Ns ") { //shininess
			std::istringstream s(line.substr(3));
			s >> material->shininess;
			material->shininess = glm::clamp(material->shininess / 1000.0f * 128.0f, 1.0f, 128.0f);
		} else if (line.substr(0, 2) == "d ") { //opacity (transparency)
			std::istringstream s(line.substr(2));
			s >> material->opacity;
		}
	}

	
	if (material != nullptr) { //add the last material if it exists
		mesh->current_scene->materials.push_back(material);
		if (material->name == "bone") {
			
		}
	}

	file.close();
}


Texture* ObjectParser::ParseTxtFile(const std::string& filepath, Mesh*& mesh, Material*& material, TEXTURE_TYPES type) {
	std::string fp = "/data/textures/" + filepath;
	std::string absolute_filepath = std::filesystem::current_path().string() + fp;
	std::cout << "TEXTURE FILEPATH: " << absolute_filepath << std::endl;
	Texture* txt = new Texture(absolute_filepath, type);
	txt->index = mesh->current_scene->textures.size();
	mesh->current_scene->textures.push_back(txt);
	txt->LoadTexture();
	return txt;
}

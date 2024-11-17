#pragma once
#include <string>
#include <vector>
#include "mesh.h"
#include "json.hpp"
struct ObjectParser {
	
	static void ParseObjFile(const std::string& filepath, Mesh*& mesh);
	static void ParseVertex(const std::string& line, Mesh*& mesh);
	static void ParseNormal(const std::string& line, Mesh*& mesh, int idx);
	static void ParseTextureCoord(const std::string& line, Mesh*& mesh, int idx);
	static void ParseFace(const std::string& line, Mesh*& mesh);
	static void ParseEdge(const std::string& line, Mesh*& mesh);
	static void ParseMtlFile(const std::string& filepath, Mesh*& mesh);
	static Texture* ParseTxtFile(const std::string& filepath, Mesh*& mesh, Material*& material, TEXTURE_TYPES type);
	static std::vector<std::string> Split(const std::string& str, char delimiter);
	
};

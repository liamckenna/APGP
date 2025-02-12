#include "material.h"
#include <iostream>
Material::Material(const nlohmann::json& data) {
	
	name = data["name"];
	index = 0;
	colors.dif = glm::vec3(data["dif"][0], data["dif"][1], data["dif"][2]);
	colors.amb = glm::vec3(data["amb"][0], data["amb"][1], data["amb"][2]);
	colors.spc = glm::vec3(data["spc"][0], data["spc"][1], data["spc"][2]);
	colors.ems = glm::vec3(data["ems"][0], data["ems"][1], data["ems"][2]);
	shininess = data["shininess"];
	roughness = data["roughness"];
	opacity = data["opacity"];
	metallic = data["metallic"];
	refractive_index = data["refractive_index"];
	tris = 0;
	edges = 0;
	textures.dif = nullptr;
	textures.nrm = nullptr;
	textures.bmp = nullptr;
	textures.spc = nullptr;
	textures.rgh = nullptr;
	textures.dsp = nullptr;
	textures.aoc = nullptr;
	textures.opc = nullptr;
	textures.ems = nullptr;
	textures.hgt = nullptr;
	textures.met = nullptr;
}

Material::Material() {
	colors.dif 	= glm::vec3(1.f, 1.f, 1.f);
	colors.amb 	= glm::vec3(0.2f, 0.2f, 0.2f);
	colors.spc 	= glm::vec3(1.f, 1.f, 1.f);
	colors.ems 	= glm::vec3(0.f, 0.f, 0.f);
	shininess  	= 32.f;
	roughness 	= 0.5f;
	opacity		= 1.f;
	metallic	= 0.f;
	tris 		= 0;
	edges		= 0;
	refractive_index = 1.f;
	textures.dif = nullptr;
	textures.nrm = nullptr;
	textures.bmp = nullptr;
	textures.spc = nullptr;
	textures.rgh = nullptr;
	textures.dsp = nullptr;
	textures.aoc = nullptr;
	textures.opc = nullptr;
	textures.ems = nullptr;
	textures.hgt = nullptr;
	textures.met = nullptr;
}
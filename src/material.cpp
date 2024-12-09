#include "material.h"

Material::Material() {
	colors.dif 	= glm::vec3(1.f, 1.f, 1.f);
	colors.amb 	= glm::vec3(0.2f, 0.2f, 0.2f);
	colors.spc 	= glm::vec3(1.f, 1.f, 1.f);
	colors.ems 	= glm::vec3(0.f, 0.f, 0.f);
	shininess  	= 32.f;
	roughness 	= 1.f;
	opacity		= 1.f;
	metallic	= 0.f;
	tris 		= 0;
	edges		= 0;
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
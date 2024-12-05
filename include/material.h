#pragma once
#include "texture.h"
#include <glm/vec3.hpp>
#include <string>
struct Material {

	std::string name;
	int index;

	struct Colors {
		glm::vec3 dif;
		glm::vec3 amb;
		glm::vec3 spc;
		glm::vec3 ems;

	} colors;

	float shininess; //phong exp
	float glossiness;
	float opacity;
	int tris;
	int edges;


	struct Textures {
		Texture* dif; //diffuse (albedo)
		Texture* nrm; //normal
		Texture* bmp; //bump
		Texture* spc; //specular
		Texture* gls; //glossiness (roughness)
		Texture* dsp; //displacement
		Texture* aoc; //ambient occlusion
		Texture* opc; //opacity (alpha)
		Texture* ems; //emissive
		Texture* hgt; //height
	} textures;	

	Material();


};
#pragma pack(push, 1)
struct FlattenedMaterial {
	int index;				//4 bytes
	int tris;				//4 bytes
	int edges;				//4 bytes
	float pad1;				//4 bytes padding (total 16 bytes)

	glm::vec3 dif_color;	//12 bytes
	float pad2;				//4 bytes padding (total 16 bytes)

	glm::vec3 amb_color;	//12 bytes
	float pad3;				//4 bytes padding (total 16 bytes)

	glm::vec3 spc_color;	//12 bytes
	float pad4;				//4 bytes padding (total 16 bytes)

	glm::vec3 ems_color;	//12 bytes
	float pad5;				//4 bytes padding (total 16 bytes)

	float shininess;		//4 bytes
	float glossiness;		//4 bytes
	float opacity;			//4 bytes
	float pad6;				//4 bytes padding (total 16 bytes)

	int dif_texture_index;	//4 bytes
	int nrm_texture_index;	//4 bytes
	int bmp_texture_index;	//4 bytes
	int spc_texture_index;	//4 bytes (total 16 bytes)

	int gls_texture_index;	//4 bytes
	int dsp_texture_index;	//4 bytes
	int aoc_texture_index;	//4 bytes
	int opc_texture_index;	//4 bytes (total 16 bytes)

	int ems_texture_index;	//4 bytes
	int hgt_texture_index;	//4 bytes
	int pad7;				//4 bytes padding
	int pad8;				//4 bytes padding (total 16 bytes)
};
#pragma pack(pop)
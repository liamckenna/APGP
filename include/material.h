#pragma once
#include "texture.h"
#include <glm/vec3.hpp>
#include <string>
#include "json.h"

struct Scene;

struct Material {
    std::string name;
    int index = -1;  // Matches the `MaterialQuery()` lookup index

    Scene* scene = nullptr;

    struct Colors {
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 ambient = glm::vec3(0.2f);
        glm::vec3 specular = glm::vec3(1.0f);
        glm::vec3 emissive = glm::vec3(0.0f);
    } colors;

    float shininess = 32.0f; // Phong exponent
    float roughness = 0.5f;
    float opacity = 1.0f;
    float metallic = 0.0f;
    float refractive_index = 1.0f;

    struct TextureSlots {
        int diffuse = -1; // Diffuse (albedo)
        int normal = -1;  // Normal
        int bump = -1;    // Bump
        int specular = -1;// Specular
        int roughness = -1;// Roughness (glossiness inverse)
        int displacement = -1;
        int ambient_occlusion = -1;
        int opacity = -1; // Opacity (transparency inverse, alpha)
        int emissive = -1;
        int height = -1;
        int metallic = -1;
    } textures;

    Material() = default;
    Material(const nlohmann::json& data, Scene* scene);
};
#pragma pack(push, 1)
struct FlattenedMaterial {
	int index;				//4 bytes
	int tris;				//4 bytes
	int edges;				//4 bytes
	float refractive_index;	//4 bytes (total 16 bytes)

	glm::vec3 dif_color;	//12 bytes
	float pad2;				//4 bytes padding (total 16 bytes)

	glm::vec3 amb_color;	//12 bytes
	float pad3;				//4 bytes padding (total 16 bytes)

	glm::vec3 spc_color;	//12 bytes
	float pad4;				//4 bytes padding (total 16 bytes)

	glm::vec3 ems_color;	//12 bytes
	float pad5;				//4 bytes padding (total 16 bytes)

	float shininess;		//4 bytes
	float roughness;		//4 bytes
	float opacity;			//4 bytes
	float metallic;			//4 bytes (total 16 bytes)

	int dif_texture_index;	//4 bytes
	int nrm_texture_index;	//4 bytes
	int bmp_texture_index;	//4 bytes
	int spc_texture_index;	//4 bytes (total 16 bytes)

	int rgh_texture_index;	//4 bytes
	int dsp_texture_index;	//4 bytes
	int aoc_texture_index;	//4 bytes
	int opc_texture_index;	//4 bytes (total 16 bytes)

	int ems_texture_index;	//4 bytes
	int hgt_texture_index;	//4 bytes
	int met_texture_index;	//4 bytes padding
	int pad8;				//4 bytes padding (total 16 bytes)
};
#pragma pack(pop)
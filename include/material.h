#pragma once
#include <glm/vec3.hpp>
#include <string>
#include "json.h"

struct Material {
    std::string name;
    int index = -1;

    struct Colors {
		glm::vec3 ambient = glm::vec3(0.2f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);
    } colors;

    float shininess = 32.0f; //phong exp
	float alpha = 1.0f;

    struct TextureSlots {
        int ambient = -1;
        int diffuse = -1;
        int specular = -1;
        int emissive = -1;
        int alpha = -1;
		int decal = -1;
		int shininess = -1;
        int bump = -1;
        int displacement = -1;
		int reflection = -1;
    } textures;

    Material() = default;
};

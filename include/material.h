#pragma once
#include <glm/vec3.hpp>
#include <string>
#include "json.h"

struct Material {
    std::string name;
    int index = -1;
    float shininess = 32.0f;
    float alpha = 1.0f;
    float metallic = 0.0f;
    float roughness = 0.5f;

    struct Colors {
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);
        glm::vec3 emissive = glm::vec3(0.0f);
    } colors;


    struct TextureSlots {
        int diffuse = -1;
        int specular = -1;
        int emissive = -1;
        int shininess = -1;
        int normal = -1;
        int roughness = -1;
        int metallic = -1;
        int ao = -1;
        int bump = -1;
        int displacement = -1;
        int reflection = -1;
        int opacity = -1;
    } textures;

    Material() = default;
};

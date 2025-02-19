#pragma once
#include <glm/vec3.hpp>


struct FlatMaterial
{
    int index;
    float shininess;
    float alpha;
    float pad1;

    glm::vec3 ambient;
    float pad2;

    glm::vec3 diffuse;
    float pad3;

    glm::vec3 specular;
    float pad4;

    int ambient_tex;
    int diffuse_tex;
    int specular_tex;
    int emissive_tex;
    
    int shininess_tex;
    int bump_tex;
    int displacement_tex;
    int reflection_tex;

};

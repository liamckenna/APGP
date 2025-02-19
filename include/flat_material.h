#pragma once
#include <glm/vec3.hpp>


#pragma pack(push, 1)
struct FlatMaterial
{
    int index;
    int pad1;
    int pad2;
    int pad3;

    glm::vec3 ambient;
    float pad4;

    glm::vec3 diffuse;
    float pad5;

    glm::vec3 specular;
    float pad6;

    float shininess;
    float alpha;
    float pad7;
    float pad8;

    int ambient_tex;
    int diffuse_tex;
    int specular_tex;
    int emissive_tex;

    int alpha_tex;
    int decal_tex;
    int shininess_tex;
    int bump_tex;

    int displacement_tex;
    int reflection_tex;
    int padX1;
    int padX2;
};
#pragma pack(pop)

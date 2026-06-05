#pragma once
#include <glm/vec3.hpp>


struct FlatMaterial
{
    int index;
    float shininess;
    float alpha;
    float pad1;

    glm::vec3 diffuse;
    float metallic;

    glm::vec3 specular;
    float roughness;

    glm::vec3 emissive;
    float pad4;

    int diffuse_tex;
    int specular_tex;
    int emissive_tex;
    int shininess_tex;

    int normal_tex;
    int roughness_tex;
    int metallic_tex;
    int ao_tex;

    int bump_tex;
    int displacement_tex;
    int reflection_tex;
    int opacity_tex;

};

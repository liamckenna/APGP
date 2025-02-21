#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include "mesh.h"
#include "material.h"
#include "flat_material.h"
#include "texture.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"


class ResourceManager {
public:

    std::unordered_map<std::string, Mesh> meshes;

    std::vector<Material> materials;
    std::unordered_map<std::string, int> material_map;
    GLuint material_ubo;

    std::vector<Texture> textures;
    std::unordered_map<std::string, GLuint> texture_map;

    Mesh& GetMesh(const std::string& name) {
        if (meshes.find(name) == meshes.end()) {
            LoadObjectFile(name);
        }
        return meshes[name];
    }

    Material& GetMaterial(const std::string& name) {
        return materials[MaterialQuery(name)];
    }

    Texture& GetTexture(const std::string& name) {
        return textures[TextureQuery(name)];
    }

    void Clear() {
        meshes.clear();
        materials.clear();
        material_map.clear();
        textures.clear();
        texture_map.clear();
    }

    ResourceManager() {

        glGenBuffers(1, &material_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, material_ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(FlatMaterial) * MAX_MATERIALS, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BINDING_POINT, material_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


        //LoadObjectFile("default");
        
    };

    std::vector<FlatMaterial> FlattenMaterials() {

        std::vector<FlatMaterial> flat_materials;
        flat_materials.reserve(materials.size());

        for (const auto& material : materials) {
            FlatMaterial flat_material{};

            flat_material.index = material.index;
            flat_material.shininess = material.shininess;
            flat_material.alpha = material.alpha;

            flat_material.ambient = material.colors.ambient;

            flat_material.diffuse = material.colors.diffuse;
            
            flat_material.specular = material.colors.specular;

            flat_material.ambient_tex = material.textures.ambient;
            flat_material.diffuse_tex = material.textures.diffuse;
            flat_material.specular_tex = material.textures.specular;
            flat_material.emissive_tex = material.textures.emissive;
            flat_material.shininess_tex = material.textures.shininess;
            flat_material.bump_tex = material.textures.bump;
            flat_material.displacement_tex = material.textures.displacement;
            flat_material.reflection_tex = material.textures.reflection;
            flat_materials.push_back(flat_material);
        }

        return flat_materials;

    }

    void UpdateMaterialBuffer() {
        std::vector<FlatMaterial> flat_materials = FlattenMaterials();
        glBindBuffer(GL_UNIFORM_BUFFER, material_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, flat_materials.size() * sizeof(FlatMaterial), flat_materials.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

    }

    int MaterialQuery(const std::string& name) {
        if (material_map.find(name) != material_map.end()) {
            return material_map[name];
        }
        else {
            std::cerr << "Warning: Material " << name << " not found. Using default." << std::endl;
            return 0;
        }
    }
    int TextureQuery(const std::string& name) {
        if (texture_map.find(name) != texture_map.end()) {
            return texture_map[name];
        }
        
        LoadTextureFile(name);

        if (texture_map.find(name) != texture_map.end()) {
            return texture_map[name];
        }

        std::cerr << "Warning: Texture " << name << " not found. Using default." << std::endl;
        return 0;
    }

    void LoadObjectFile(const std::string& filename);
    void LoadMaterialFile(const std::string& filename);
    void LoadTextureFile(const std::string& filename);
};
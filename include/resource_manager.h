#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include "mesh.h"
#include "material.h"
#include "texture.h"

class ResourceManager {
public:
    std::unordered_map<std::string, Mesh> meshes;

    std::vector<Material> materials;
    std::unordered_map<std::string, int> material_map;

    std::vector<Texture> textures;
    std::unordered_map<std::string, int> texture_map;

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
        //get mesh and texture files from here
        GetMaterial("default");
        GetMesh("data/objects/default.obj");
        GetTexture("data/textures/default.png");
    };

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
        else {
            std::cerr << "Warning: Texture " << name << " not found. Using default." << std::endl;
            return 0;
        }
    }

    void LoadObjectFile(const std::string& filepath);
    void LoadMaterialFile(const std::string& filepath);
};

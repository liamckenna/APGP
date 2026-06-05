#include "resource_manager.h"
#include "mesh.h"
#include "surface.h"
#include "vertex.h"
#include "material.h"
#include "texture.h"
#include "vertex_key.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void ResourceManager::LoadObjectFile(const std::string& filename) {
    std::string filepath = "data/objects/" + filename + ".obj";
    std::ifstream file(filepath);

    Mesh mesh;
    mesh.name = filename;

    if (!file.is_open()) {
        std::cerr << "Failed to open .obj file: " << filepath << std::endl;
        return;
    }

    std::string line;
    std::string material_file;
    int material_index = 0;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    int line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "mtllib") {
            iss >> material_file;

            std::ifstream testMaterialFile("data/materials/" + material_file);
            if (testMaterialFile.good()) {
                LoadMaterialFile(material_file);
            }
            else {
                std::cerr << "Warning: Material file not found: " << material_file << std::endl;
            }
        }
        else if (keyword == "usemtl") {
            std::string material_name;
            iss >> material_name;
            material_name = material_file + "." + material_name;
            material_index = MaterialQuery(material_name);

        }
        else if (keyword == "v") {
            glm::vec3 position;
            iss >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (keyword == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (keyword == "vt") {
            glm::vec2 texcoord;
            iss >> texcoord.x >> texcoord.y;
            texcoords.push_back(texcoord);
        }
        else if (keyword == "f") {
            std::vector<int> vertexIndices;
            std::string vertexData;

            while (iss >> vertexData) {
                std::istringstream viss(vertexData);
                std::string posIdx, texIdx, normIdx;

                std::getline(viss, posIdx, '/');
                std::getline(viss, texIdx, '/');
                std::getline(viss, normIdx, '/');

                int vIndex = std::stoi(posIdx) - 1;
                int tIndex = texIdx.empty() ? -1 : std::stoi(texIdx) - 1;
                int nIndex = normIdx.empty() ? -1 : std::stoi(normIdx) - 1;

                if (vIndex < 0 || vIndex >= positions.size()) {
                    std::cerr << "Invalid position index: " << vIndex << std::endl;
                    continue;
                }
                if (tIndex != -1 && (tIndex < 0 || tIndex >= texcoords.size())) {
                    std::cerr << "Invalid texcoord index: " << tIndex << std::endl;
                    continue;
                }
                if (nIndex != -1 && (nIndex < 0 || nIndex >= normals.size())) {
                    std::cerr << "Invalid normal index: " << nIndex << std::endl;
                    continue;
                }

                VertexKey key{ vIndex, tIndex, nIndex };

                if (mesh.unique_vertices.find(key) == mesh.unique_vertices.end()) {
                    Vertex vertex;
                    vertex.position = positions[vIndex];
                    vertex.texcoord = (tIndex != -1) ? texcoords[tIndex] : glm::vec2(0.0f);
                    vertex.normal = (nIndex != -1) ? normals[nIndex] : glm::vec3(0.0f);

                    int newIndex = static_cast<int>(mesh.vertices.size());
                    mesh.vertices.push_back(vertex);
                    mesh.unique_vertices[key] = newIndex;
                }

                vertexIndices.push_back(mesh.unique_vertices[key]);
            }

            for (size_t i = 1; i + 1 < vertexIndices.size(); i++) {
                mesh.indices.push_back(vertexIndices[0]);
                mesh.indices.push_back(vertexIndices[i]);
                mesh.indices.push_back(vertexIndices[i + 1]);
                mesh.material_index.push_back(material_index);
            }
        }
    }

    if (normals.empty()) {
        std::cout << "Generating Normals for " << mesh.name << std::endl;
        for (size_t i = 0; i < mesh.indices.size(); i += 3) {
            int i0 = mesh.indices[i];
            int i1 = mesh.indices[i + 1];
            int i2 = mesh.indices[i + 2];

            glm::vec3 edge1 = mesh.vertices[i1].position - mesh.vertices[i0].position;
            glm::vec3 edge2 = mesh.vertices[i2].position - mesh.vertices[i1].position;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

            mesh.vertices[i0].normal += normal;
            mesh.vertices[i1].normal += normal;
            mesh.vertices[i2].normal += normal;
        }
        for (auto& vertex : mesh.vertices) {
            vertex.normal = glm::normalize(vertex.normal);
        }
    }

    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        int i0 = mesh.indices[i], i1 = mesh.indices[i + 1], i2 = mesh.indices[i + 2];
        glm::vec3 edge1 = mesh.vertices[i1].position - mesh.vertices[i0].position;
        glm::vec3 edge2 = mesh.vertices[i2].position - mesh.vertices[i0].position;
        glm::vec2 dUV1  = mesh.vertices[i1].texcoord - mesh.vertices[i0].texcoord;
        glm::vec2 dUV2  = mesh.vertices[i2].texcoord - mesh.vertices[i0].texcoord;
        float denom = dUV1.x * dUV2.y - dUV2.x * dUV1.y;
        if (std::abs(denom) < 1e-6f) continue;
        glm::vec3 t = (1.0f / denom) * (dUV2.y * edge1 - dUV1.y * edge2);
        mesh.vertices[i0].tangent += t;
        mesh.vertices[i1].tangent += t;
        mesh.vertices[i2].tangent += t;
    }
    for (auto& v : mesh.vertices)
        v.tangent = glm::normalize(v.tangent);

    mesh.SetupBuffers();

    meshes.emplace(mesh.name, std::move(mesh));
}



void ResourceManager::LoadSurfaceFile(const std::string& filename) {
    std::string filepath = "data/surfaces/" + filename + ".srf";
    std::ifstream file(filepath);

    Surface surface;
    surface.name = filename;

    if (!file.is_open()) {
        std::cerr << "Failed to open .srf file: " << filepath << std::endl;
        return;
    }

    std::string line;
    std::string material_file;
    int material_index = 0;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    int line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "mtllib") {
            iss >> material_file;

            std::ifstream testMaterialFile("data/materials/" + material_file);
            if (testMaterialFile.good()) {
                LoadMaterialFile(material_file);
            }
            else {
                std::cerr << "Warning: Material file not found: " << material_file << std::endl;
            }
        }
        else if (keyword == "usemtl") {
            std::string material_name;
            iss >> material_name;
            material_name = material_file + "." + material_name;
            material_index = MaterialQuery(material_name);

        }
        else if (keyword == "v") {
            glm::vec3 position;
            iss >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (keyword == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (keyword == "vt") {
            glm::vec2 texcoord;
            iss >> texcoord.x >> texcoord.y;
            texcoords.push_back(texcoord);
        }
        else if (keyword == "ps") {
            int patch_size;
			iss >> patch_size;
            surface.patch_size = patch_size;
        }
        else if (keyword == "cv") {
            std::vector<int> vertexIndices;
            std::string vertexData;

            while (iss >> vertexData) {
                std::istringstream viss(vertexData);
                std::string posIdx, texIdx, normIdx;

                std::getline(viss, posIdx, '/');
                std::getline(viss, texIdx, '/');
                std::getline(viss, normIdx, '/');

                int vIndex = std::stoi(posIdx);
                int tIndex = texIdx.empty() ? -1 : std::stoi(texIdx) - 1;
                int nIndex = normIdx.empty() ? -1 : std::stoi(normIdx) - 1;

                if (vIndex < 0 || vIndex >= positions.size()) {
                    std::cerr << "Invalid position index: " << vIndex << std::endl;
                    continue;
                }
                if (tIndex != -1 && (tIndex < 0 || tIndex >= texcoords.size())) {
                    std::cerr << "Invalid texcoord index: " << tIndex << std::endl;
                    continue;
                }
                if (nIndex != -1 && (nIndex < 0 || nIndex >= normals.size())) {
                    std::cerr << "Invalid normal index: " << nIndex << std::endl;
                    continue;
                }

                VertexKey key{ vIndex, tIndex, nIndex };

                if (surface.unique_vertices.find(key) == surface.unique_vertices.end()) {
                    Vertex vertex;
                    vertex.position = positions[vIndex];
                    vertex.texcoord = (tIndex != -1) ? texcoords[tIndex] : glm::vec2(0.0f);
                    vertex.normal = (nIndex != -1) ? normals[nIndex] : glm::vec3(0.0f);

                    int newIndex = static_cast<int>(surface.vertices.size());
                    surface.vertices.push_back(vertex);
                    surface.unique_vertices[key] = newIndex;
                }
                vertexIndices.push_back(surface.unique_vertices[key]);
            }
            if (vertexIndices.size() == 4) {
                surface.indices.push_back(vertexIndices[0]);
                surface.indices.push_back(vertexIndices[1]);
                surface.indices.push_back(vertexIndices[2]);
                surface.indices.push_back(vertexIndices[3]);
                surface.material_index.push_back(material_index);
            }
        }
    }
    //just in case
    if (normals.empty()) {
        std::cout << "Generating Normals for " << surface.name << std::endl;
        for (size_t i = 0; i < surface.vertices.size(); i++) {

            surface.vertices[i].normal = glm::vec3(0.f);

        }   
    }

    surface.SetupBuffers();

    surfaces.emplace(surface.name, std::move(surface));
}



void ResourceManager::LoadMaterialFile(const std::string& filename) {
    std::string file_path = "data/materials/" + filename;
    std::ifstream file(file_path);

    if (material_map.find(filename) != material_map.end()) {
        return;
    }

    if (!file.is_open()) {
        std::cerr << "Failed to open material file: " << file_path << std::endl;
        return;
    }

    std::string line;
    std::string current_material_name;
    Material current_material;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "newmtl") {
            if (!current_material_name.empty()) {
                material_map[current_material_name] = materials.size();
                materials.emplace_back(std::move(current_material));
            }

            iss >> current_material_name;
            current_material_name = filename + "." + current_material_name;
            current_material = Material(); // Reset for a new material
            current_material.name = current_material_name;
            current_material.index = materials.size();
        }
        else if (keyword == "Kd") { // Diffuse color
            iss >> current_material.colors.diffuse.x >> current_material.colors.diffuse.y >> current_material.colors.diffuse.z;
        }
        else if (keyword == "Ks") { // Specular color
            iss >> current_material.colors.specular.x >> current_material.colors.specular.y >> current_material.colors.specular.z;
        }
        else if (keyword == "Ke") { // Emissive color
            iss >> current_material.colors.emissive.x >> current_material.colors.emissive.y >> current_material.colors.emissive.z;
        }
        else if (keyword == "Ns") { // Shininess
            iss >> current_material.shininess;
        }
        else if (keyword == "d") { // Opacity
            iss >> current_material.alpha;
        }
        else if (keyword == "Pr") { // Roughness (PBR)
            iss >> current_material.roughness;
        }
        else if (keyword == "Pm") { // Metallic (PBR)
            iss >> current_material.metallic;
        }
        else if (keyword == "map_Kd") { // Diffuse texture
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.diffuse = TextureQuery(texture_name);
        }
        else if (keyword == "map_Ks") { // Specular texture
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.specular = TextureQuery(texture_name);
        }
        else if (keyword == "map_Ke") { // Emissive texture
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.emissive = TextureQuery(texture_name);
        }
        else if (keyword == "map_Ns") { // Shininess texture
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.shininess = TextureQuery(texture_name);
        }
        else if (keyword == "norm") { // Normal map (PBR)
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.normal = TextureQuery(texture_name);
        }
        else if (keyword == "map_Pr") { // Roughness texture (PBR)
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.roughness = TextureQuery(texture_name);
        }
        else if (keyword == "map_Pm") { // Metallic texture (PBR)
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.metallic = TextureQuery(texture_name);
        }
        else if (keyword == "map_ao") { // Ambient occlusion texture
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.ao = TextureQuery(texture_name);
        }
        else if (keyword == "map_d") { // Opacity texture
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.opacity = TextureQuery(texture_name);
        }
        else if (keyword == "map_bump" || keyword == "bump") {
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.bump = TextureQuery(texture_name);
        }
        else if (keyword == "disp") {
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.displacement = TextureQuery(texture_name);
        }
        else if (keyword == "refl") {
            std::string texture_name;
            std::getline(iss, texture_name);
            texture_name.erase(0, texture_name.find_first_not_of(" \t"));
            current_material.textures.reflection = TextureQuery(texture_name);
        }
    }
    if (!current_material_name.empty()) {
        material_map[current_material_name] = materials.size();
        materials.emplace_back(std::move(current_material));
    }

    UpdateMaterialBuffer();

}

void ResourceManager::LoadTextureFile(const std::string& filename) {
    std::string file_path = "data/textures/" + filename;

    if (texture_map.find(filename) != texture_map.end()) {
        std::cout << "fild found already!" << std::endl;
        return;
    }

    Texture texture;
    texture.name = filename;
    texture.index = textures.size();
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(file_path.c_str(),
        &texture.width,
        &texture.height,
        &texture.channels,
        0);
    if (!data) {
        std::cerr << "Error: Failed to load texture: " << file_path << std::endl;
        return; // bail out
    }

    glGenTextures(1, &texture.id);
    glActiveTexture(GL_TEXTURE2 + texture.index);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    GLenum format = GL_RGBA;
    if (texture.channels == 1)       format = GL_RED;
    else if (texture.channels == 3)  format = GL_RGB;
    else if (texture.channels == 4)  format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format,
        texture.width, texture.height,
        0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    texture.is_loaded = true;
    std::cout << "texture loaded: " << texture.name << std::endl;

    texture_map[filename] = textures.size();
    textures.emplace_back(std::move(texture));
}

#include "resource_manager.h"
#include "mesh.h"
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
    mesh.material_index.push_back(material_index);
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
            std::cout << material_name << " at line " << line_number << std::endl;
            std::cout << "previous index: " << material_index << std::endl;
            material_index = MaterialQuery(material_name);
            std::cout << "new index: " << material_index << std::endl;

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

            if (vertexIndices.size() == 3) {
                mesh.indices.push_back(vertexIndices[0]);
                mesh.indices.push_back(vertexIndices[1]);
                mesh.indices.push_back(vertexIndices[2]);
                std::cout << "adding face" << std::endl;
                mesh.material_index.push_back(material_index);
            }
            else if (vertexIndices.size() == 4) {
                mesh.indices.push_back(vertexIndices[0]);
                mesh.indices.push_back(vertexIndices[1]);
                mesh.indices.push_back(vertexIndices[2]);
                mesh.material_index.push_back(material_index);

                mesh.indices.push_back(vertexIndices[0]);
                mesh.indices.push_back(vertexIndices[2]);
                mesh.indices.push_back(vertexIndices[3]);
                mesh.material_index.push_back(material_index);
            }
        }
    }

    if (normals.empty()) {
        for (size_t i = 0; i < mesh.indices.size(); i += 3) {
            int i0 = mesh.indices[i];
            int i1 = mesh.indices[i + 1];
            int i2 = mesh.indices[i + 2];

            glm::vec3 edge1 = mesh.vertices[i1].position - mesh.vertices[i0].position;
            glm::vec3 edge2 = mesh.vertices[i2].position - mesh.vertices[i1].position;
            glm::vec3 normal = glm::normalize(glm::cross(edge2, edge1));

            mesh.vertices[i0].normal += normal;
            mesh.vertices[i1].normal += normal;
            mesh.vertices[i2].normal += normal;
        }
        for (auto& vertex : mesh.vertices) {
            vertex.normal = glm::normalize(vertex.normal);
        }
    }

    mesh.SetupBuffers();

    meshes.emplace(mesh.name, std::move(mesh));
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
        else if (keyword == "Ka") { // Ambient color
            iss >> current_material.colors.ambient.x >> current_material.colors.ambient.y >> current_material.colors.ambient.z;
        }
        else if (keyword == "Kd") { // Diffuse color
            iss >> current_material.colors.diffuse.x >> current_material.colors.diffuse.y >> current_material.colors.diffuse.z;
        }
        else if (keyword == "Ks") { // Specular color
            iss >> current_material.colors.specular.x >> current_material.colors.specular.y >> current_material.colors.specular.z;
        }
        else if (keyword == "Ns") { // Shininess
            iss >> current_material.shininess;
        }
        else if (keyword == "d") { // Opacity
            iss >> current_material.alpha;
        }
        else if (keyword == "map_Ka") { // Ambient texture
            std::string texture_name;
            iss >> texture_name;
            current_material.textures.ambient = TextureQuery(texture_name);
        }
        else if (keyword == "map_Kd") { // Diffuse texture
            std::string texture_name;
            iss >> texture_name;
            current_material.textures.diffuse = TextureQuery(texture_name);
        }
        else if (keyword == "map_Ks") { // Specular texture
            std::string texture_name;
            iss >> texture_name;
            current_material.textures.specular = TextureQuery(texture_name);
        }
        else if (keyword == "map_d") { // Alpha texture
            std::string texture_name;
            iss >> texture_name;
            current_material.textures.alpha = TextureQuery(texture_name);
        }
        else if (keyword == "map_Ns") { // Shininess texture
            std::string texture_name;
            iss >> texture_name;
            current_material.textures.shininess = TextureQuery(texture_name);
        }
        else if (keyword == "map_bump" || keyword == "bump") { //default to normal map... ill worry bout it later
            std::string texture_name;
            iss >> texture_name;
            current_material.textures.bump = TextureQuery(texture_name);
        }
        else if (keyword == "disp") { //default to normal map... ill worry bout it later
            std::string texture_name;
            iss >> texture_name;
            current_material.textures.displacement = TextureQuery(texture_name);
        }
        else if (keyword == "refl") { //default to normal map... ill worry bout it later
            std::string texture_name;
            iss >> texture_name;
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
    // Begin STB loading
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file_path.c_str(),
        &texture.width,
        &texture.height,
        &texture.channels,
        0);
    if (!data) {
        std::cerr << "Error: Failed to load texture: " << file_path << std::endl;
        return; // bail out
    }

    // Generate and bind an OpenGL texture
    glGenTextures(1, &texture.id);
    glActiveTexture(GL_TEXTURE0 + texture.index);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // Determine pixel format
    GLenum format = GL_RGBA;
    if (texture.channels == 1)       format = GL_RED;
    else if (texture.channels == 3)  format = GL_RGB;
    else if (texture.channels == 4)  format = GL_RGBA;
    // ...if you want more checks, do so here...

    // Upload to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, format,
        texture.width, texture.height,
        0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    // Clean up STB data + unbind
    stbi_image_free(data);

    // Mark as loaded
    texture.is_loaded = true;
    std::cout << "texture loaded: " << texture.name << std::endl;
    // Add it to our resource arrays/maps
    texture_map[filename] = textures.size();
    textures.emplace_back(std::move(texture));
}
